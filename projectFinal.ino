#include "secrets.h"
#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial
  #define NODEBUG_WEBSOCKETS
  #define NDEBUG
#endif

#include <Arduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #define DHT_PIN D14
#endif
#ifdef ESP32
  #include <WiFi.h>
  #define DHT_PIN 14
#endif

#include "SinricPro.h"
#include "SinricProTemperaturesensor.h"
#include "DHT.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define EVENT_WAIT_TIME   60000                         
#define BAUD_RATE  115200
#define DHTTYPE DHT11
#define PUB_TOPIC "esp32/pub"
#define SUB_TOPIC "esp32/sub"

DHT dht(DHT_PIN, DHTTYPE);

float actualTemperature;
float actualHumidity;
float lastTemperature;
float lastHumidity;
unsigned long lastEvent = (-EVENT_WAIT_TIME);

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);



void handleTemperatureSensor() {
  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < EVENT_WAIT_TIME) return;

  actualTemperature = dht.readTemperature();
  actualHumidity = dht.readHumidity();

  if (isnan(actualTemperature) || isnan(actualHumidity)) { 
    Serial.printf("Sensor DHT11 nao esta lendo!\r\n");
    return;
  } 

  if (actualTemperature == lastTemperature || actualHumidity == lastHumidity) return;
  
  publishMessage();

  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
  bool success = mySensor.sendTemperatureEvent(actualTemperature, actualHumidity);
  if (success) {
    publishMessage();
    Serial.printf("Temperatura: %2.1f ºC\tUmidade: %2.1f%%\r\n", actualTemperature, actualHumidity);
  } else {
    Serial.printf("Algo deu errado... não foi possível enviar o evento para o servidor!\r\n");
  }

  lastTemperature = actualTemperature;
  lastHumidity = actualHumidity;
  lastEvent = actualMillis;
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Conectando");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("conectado!\r\n[WiFi]: Endereço de IP %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void conectAws(){
  //Conexão com AWS IOT CORE
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Conectando ao AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(SUB_TOPIC);
 
  Serial.println("AWS IoT Conectado!");
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Enviando ao IOT CORE: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["umidade"] = actualHumidity;
  doc["temperatura"] = actualTemperature;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
 
  client.publish(PUB_TOPIC, jsonBuffer);
}

void setupSinricPro() {
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
  SinricPro.onConnected([](){ Serial.printf("Conctado ao SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Desconectado do SinricPro\r\n"); });
  SinricPro.restoreDeviceStates(true);
     
  SinricPro.begin(APP_KEY, APP_SECRET);  
}

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  dht.begin();

  setupWiFi();
  conectAws();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleTemperatureSensor();
  client.loop();
}































