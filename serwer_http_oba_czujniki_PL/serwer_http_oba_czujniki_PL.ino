#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13  //piny do bmp280
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
Adafruit_BMP280 bme; //I2C init

#define DHTPIN D4     //piny do dht22
#define DHTTYPE DHT22   //DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //DHT init
 
const char* ssid = "Stacja01"; //nazwa ap sieci
const char* password = "stacja2551"; //haslo do ap
 
WiFiServer server(80);
IPAddress ip(192, 168, 1, 100); //IP stacji pogodowej
IPAddress gateway(192, 168, 1, 1); //gateway sieci
 
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
  IPAddress subnet(255, 255, 255, 0); //maska taka jak na ap
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA); //tryb polaczenai stacja-AP
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  //Start serwera
  server.begin();
  Serial.println("Server started");
 
  //Wypisz adres IP stacji
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  if (!bme.begin()) {  
    Serial.println("Nie znaleziono modulu bmp280! Sprawdz kable!");
    while (1);
  }
  
  dht.begin();
}
 
void loop() {
  //Sprawdzanie czy podlaczyl sie ktos do stacji
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  //czekaj na klienta
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  //czytaj request klienta
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  //odczyt danych z DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //odczyt danych z BMP280
  float tb = bme.readTemperature();
  float p = (bme.readPressure()/100);
  
 
  //zwroc dane
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  //temperatura z czujnika DHT22
  if (isnan(t)){
    client.print("blad odczytu temperatury");
  } 
  else{
    client.print(t);
  }
  client.print(",");

  //wilgotnosc z czujnika DHT22
  if (isnan(h)){
    client.print("blad odczytu wilgotnosci");
  } 
  else{
    client.print(h);
  }
  client.print(",");
  
  //Temperatura z czujnika BMP280
  if (isnan(tb)){
    client.print("blad odczytu temperatury z bmp280");
  } 
  else{
    client.print(tb);
  }
  client.print(","); 
  
  //Cisnienie wzgledne z czujnika BMP280
  if (isnan(p)){
    client.print("blad odczytu cisnienia");
  } 
  else{
    client.print(p);
  }
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}
