### Informe Estación Meteorológica : Servidor


El propósito de este código es el de crear un servidor web mediante una ESP32 maestra, que posteriormente se encargará de recibir datos de otra ESP32 esclava, mostrarlos en una página web, además de una lista con todos los dispotivos conectados en ese momneto.

*1. Configuración Wi-Fi y almacenamiento de datos*

La ESP32 maestra crea una red Wi-Fi que requiere contraseña para acceder, a esta se conectarán las ESP32 esclavas que transmitirán los datos a través de unas variables: 

           -float temperature = 0.0
           -float pressure = 0.0;
           -float humidity = 0.0;

*2. Página Web y Endpoints*

Se encarga de mostrar los datos recibidos como:

            - Temperatura (°C)
            - Presión (hPa)
            - Humedad (%)
            - Tabla con MACs de dispositivos conectados

Y los endpoints permiten recibir datos del esclavo, devolverlos en JSON o ir a la página principial, para ello se utilizará /submit,
/data y / respectivamente.

*3. Funcionamiento del código*

Se inicia un Access Point y un servidor
  ```
    WiFi.softAP(apSSID, apPassword);
    server.begin();
  ```

La ESP32 esclava envía los datos a /submit con 
  ```
    {"temp": 25.5, "pres": 1013, "hum": 45}
  ```

El usuario también podrá visualizarlos con /
  ```
    <p>23.5°C</p>   <!-- Temperatura -->
    <p>1013 hPa</p> <!-- Presión -->
    <p>45%</p>      <!-- Humedad -->
  ```

Y detecta los MACs de otros dispositivos conectados usando la función
  ```
    esp_wifi_ap_get_sta_list(&stations);
  ```