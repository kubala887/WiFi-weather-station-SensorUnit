#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13  //pins for bmp280 sensor
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
Adafruit_BMP280 bme; //I2C init

#define DHTPIN D4     //pins for dht22 sensor
#define DHTTYPE DHT22   //using DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //DHT init
 
const char* ssid = "Stacja01"; //name of wifi AP
const char* password = "stacja2551"; //AP password 
 
WiFiServer server(80);
IPAddress ip(192, 168, 1, 100); //IP of this station unit
IPAddress gateway(192, 168, 1, 1); //gateway
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
 
  //Laczenie z siecia
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA); //type of connection
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  //Start the server
  server.begin();
  Serial.println("Server started");
 
  //Print station IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  if (!bme.begin()) {  
    Serial.println("Cannot find bmp280 module! Check cable connection!");
    while (1);
  }
  
  dht.begin();
}
 
void loop() {
  //Check if anybody connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  //read data from DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //read data from BMP280
  float tb = bme.readTemperature();
  float p = (bme.readPressure()/100);
  
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  //Temperature from DHT22
  if (isnan(t)){
    client.print("read error dht22 temp");
  } 
  else{
    client.print(t);
  }
  client.print(",");

  //Humidity from DHT22
  if (isnan(h)){
    client.print("read error dht22 hum");
  } 
  else{
    client.print(h);
  }
  client.print(",");
  
  //Temperature from BMP280
  if (isnan(tb)){
    client.print("read error bmp280 temp");
  } 
  else{
    client.print(tb);
  }
  client.print(","); 
  
  //Pressure from BMP280
  if (isnan(p)){
    client.print("pressure error on bmp280");
  } 
  else{
    client.print(p);
  }
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}
