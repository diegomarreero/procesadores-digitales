#include <WiFi.h>
#include <WebServer.h>

// Credenciales de la red Wi-Fi
const char* ssid = "Nautilus";      // Nombre de la red Wi-Fi (SSID)
const char* password = "20000Leguas";    // Contraseña de la red Wi-Fi

WebServer server(80);  // Crea un objeto WebServer que escucha en el puerto 80 (HTTP)

// Función para manejar la solicitud en la ruta raíz ("/")
void handle_root() {
  String HTML = "<!DOCTYPE html>\
    <html>\
    <body>\
    <h1>Mi Primera Página con ESP32 - Modo Estación &#128522;</h1>\
    </body>\
    </html>";
  server.send(200, "text/html", HTML);  // Envía la respuesta HTTP con el código 200 (OK) y el contenido HTML
}

void setup() {
  // Inicia la comunicación serial a 115200 baudios
  Serial.begin(115200);
  Serial.println("Intentando conectar a: ");
  Serial.println(ssid);

  // Conecta a la red Wi-Fi
  WiFi.begin(ssid, password);

  // Espera hasta que la conexión Wi-Fi sea exitosa
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Una vez conectado, muestra la dirección IP asignada
  Serial.println("");
  Serial.println("WiFi conectado exitosamente");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Asocia la función handle_root a la ruta raíz ("/")
  server.on("/", handle_root);

  // Inicia el servidor web
  server.begin();
  Serial.println("Servidor HTTP iniciado");
  delay(100);  // Pequeño retardo para estabilizar
}

void loop() {
  // Maneja las solicitudes de los clientes
  server.handleClient();
}