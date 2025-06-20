### Informe Estación Meteorológica : ESP32 Esclava

El proposito del código es recoger los datos a través de los sensores y enviarlos a las ESP32 maestra via red WiFi.

*1. Hardware*
    -Sensor BME280 para la temperatura, presión y humedad

*2. Funciones principales*

Los sensores leen los datos ambiantales y mediante el método POST, en HTTP. 

*3. Funcionamiento del código*

En el setup(), o inicialización se acativa el sensor y se conecta la ESP32 a la red WiFi.
  ```
    bme.begin(0x76);  // Inicia sensor
    WiFi.begin(ssid, password);  // Conecta a WiFi
  ```

Después procede con la lectura de datos, además de la conversión de Pa a hPa para la presión.
  ```
    float temp = bme.readTemperature();    // °C
    float pres = bme.readPressure()/100;   // Pa → hPa
    float hum = bme.readHumidity();        // %
  ```

Estos se enviarán posteriormente al servidor para ser mostrados por la página web.
Por último el loop(), que funciona como se observa en el siguiente esquema:
![ESQUEMA](deepseek_mermaid_20250604_97dae2.jpg)

Se comprueba que el WiFi esté conectado, después lee los datos del sensor y se envian a por POST HTTP. Por último espera 1s y repite.