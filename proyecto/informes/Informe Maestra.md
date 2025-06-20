### Informe Estación Meteorológica : ESP32 Maestra

El proposito de este código es el de recibir y mostrar por pantalla los datos que recolecte la ESP32 esclava, además de imprimir junto a estos gráficas que acompañan a los valores actualizados.

*1. Hardware*

    -Pantalla TFT ILI9341 (240x320 píxeles)
    -Sensor AHT10 (temperatura/humedad)
    -Reloj RTC (DS3231) para la hora actual

*2. Conexión*
Una vez creada y conectada a la red wifi obtiene los datos del servidor *http://192.168.4.1/data* y los muestra por pantalla como:

    INTERIOR:
    Temperatura (desde AHT10 local)
    ---
    EXTERIOR:
    Temperatura (desde servidor)
    Presión    (desde servidor)
    Humedad    (desde servidor)

*3. Visualización*

En cuanto la visualización de los datos, estos se retransmitirán a través del display o pantalla, son gráficos en tiempo real para cada variable que se actualizan cada 2 segundos, también muestra la hora actual gracias a su reloj RTC

*4. Funcionamiento del código*

En el Setup se inicializan la pantalla además de los sensores, se conecta a la red WiFi y dibuja la interfaz (etiquetas, bordes, etc). Después, el loop principal consiste en:
  ```
    Actualizar hora (cada minuto)
    Leer temperatura interna (AHT10)
    Obtener datos externos (HTTP GET /data)
    Actualizar valores en pantalla
   Dibujar 4 gráficos (temp int/ext, presión, humedad)
  ```

Por último, los gráficos dinámicos, que almacenan hasta 180 valores históricos, escala automática segun valores mínimos y máximos, junto a colores diferenciados para cada valor.