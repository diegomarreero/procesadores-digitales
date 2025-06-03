#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>

WebServer server(80);

const char* apSSID = "ESP32-AP";
const char* apPassword = "password";

// Variables para almacenar los datos del sensor
float temperature = 0.0;
float pressure = 0.0;
float humidity = 0.0;

String formatMAC(uint8_t mac[6]) {
  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Estacion Meteorologica</title>
    <meta http-equiv="refresh" content="5">
    <style>
      body { font-family: Arial, sans-serif; margin: 20px; }
      .data-container { display: flex; gap: 20px; margin-bottom: 30px; }
      .data-box { background: #f0f0f0; padding: 20px; border-radius: 10px; }
      table { border-collapse: collapse; width: 100%; }
      th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
      th { background-color: #4CAF50; color: white; }
    </style>
  </head>
  <body>
    <h1>Datos Meteorologicos</h1>
    
    <div class="data-container">
      <div class="data-box">
        <h2>Temperatura</h2>
        <p>%TEMPERATURA% C</p>
      </div>
      <div class="data-box">
        <h2>Presion</h2>
        <p>%PRESION% hPa</p>
      </div>
      <div class="data-box">
        <h2>Humedad</h2>
        <p>%HUMEDAD% %</p>
      </div>
    </div>

    <h2>Dispositivos Conectados</h2>
    <table>
      <tr><th>MAC Address</th></tr>
      %CLIENTES%
    </table>
  </body>
  </html>
  )rawliteral";

  // Actualizar valores
  html.replace("%TEMPERATURA%", String(temperature));
  html.replace("%PRESION%", String(pressure));
  html.replace("%HUMEDAD%", String(humidity));
  
  // Listar dispositivos conectados
  String clientList = "";
  wifi_sta_list_t stations;
  esp_wifi_ap_get_sta_list(&stations);
  
  for (int i = 0; i < stations.num; i++) {
    clientList += "<tr><td>" + formatMAC(stations.sta[i].mac) + "</td></tr>";
  }
  html.replace("%CLIENTES%", clientList);

  server.send(200, "text/html", html);
}

void handleDataSubmission() {
  if (server.hasArg("temp") && server.hasArg("pres") && server.hasArg("hum")) {
    temperature = server.arg("temp").toFloat();
    pressure = server.arg("pres").toFloat();
    humidity = server.arg("hum").toFloat();
    server.send(200, "text/plain", "Datos actualizados");
  } else {
    server.send(400, "text/plain", "Datos incompletos");
  }
}

void handleAPIData() {
  String json = "{";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"pressure\":" + String(pressure) + ",";
  json += "\"humidity\":" + String(humidity);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(apSSID, apPassword);
  
  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleDataSubmission);  // Para enviar datos
  server.on("/data", handleAPIData);                     // Para leer datos
  
  server.begin();
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}