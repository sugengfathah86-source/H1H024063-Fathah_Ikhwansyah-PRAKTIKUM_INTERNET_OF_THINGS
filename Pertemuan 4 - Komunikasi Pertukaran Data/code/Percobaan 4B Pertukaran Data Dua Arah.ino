#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h> 
#include <DHT.h> 

const char* ssid          = "UdinPetot"; 
const char* password      = "Admin1234"; 
const char* mqttServer    = "broker.hivemq.com"; 
const int   mqttPort      = 1883; 

const char* topicData     = "unsoed/tk245004/kelompokfathahnabil/data"; 
const char* topicPerintah = "unsoed/tk245004/kelompokfathahnabil/perintah"; 

#define DHTPIN 4          // Pin D2 pada NodeMCU ESP8266 (GPIO 4)
#define DHTTYPE DHT11     // Sensor DHT11

const int ledPin = D6;     // Pin D6 pada NodeMCU ESP8266 (GPIO 12)

DHT dht(DHTPIN, DHTTYPE); 
WiFiClient espClient; 
PubSubClient client(espClient); 

unsigned long waktuTerakhirPublish = 0; 
const long intervalPublish = 5000; 

void callback(char* topic, byte* payload, unsigned int length) { 
  String pesan; 
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i]; 
  }
  
  JsonDocument doc; 
  if (deserializeJson(doc, pesan)) return; 
  
  const char* perintah = doc["perintah"]; 
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW); 
  
  Serial.print("Perintah diterima -> Aktuator: "); 
  Serial.println(perintah); 
} 

void hubungkanWiFi() { 
  WiFi.begin(ssid, password); 
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!"); 
} 

void hubungkanMQTT() { 
  while (!client.connected()) { 
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX); 
    
    if (client.connect(clientId.c_str())) { 
      client.subscribe(topicPerintah); 
      Serial.println("Terhubung dan subscribe topic perintah"); 
    } else { 
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000); 
    } 
  } 
} 

void setup() { 
  Serial.begin(115200); 
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW);
  
  dht.begin(); 
  delay(2000); // Memberikan jeda 2 detik agar sensor DHT siap
  
  hubungkanWiFi(); 
  client.setServer(mqttServer, mqttPort); 
  client.setCallback(callback); 
} 

void loop() { 
  if (!client.connected()) {
    hubungkanMQTT(); 
  }
  client.loop(); 
  
  if (millis() - waktuTerakhirPublish > intervalPublish) { 
    waktuTerakhirPublish = millis(); 
    
    // Membaca suhu dari sensor DHT11
    float suhu = dht.readTemperature(); 
    
    // Jika masih gagal (NaN), coba baca lagi sekali lagi
    if (isnan(suhu)) {
      delay(100);
      suhu = dht.readTemperature();
    }

    if (!isnan(suhu)) { 
      JsonDocument doc; 
      doc["suhu"] = suhu; 
      
      char buffer[128]; 
      serializeJson(doc, buffer); 
      
      client.publish(topicData, buffer); 
      Serial.print("Data terkirim: "); 
      Serial.println(buffer); 
    } else {
      Serial.println("Gagal membaca dari sensor DHT!");
    }
  } 
}