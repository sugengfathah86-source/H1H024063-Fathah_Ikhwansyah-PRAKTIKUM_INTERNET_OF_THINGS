#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h> 

const char* ssid         = "UdinPetot"; 
const char* password     = "Admin1234"; 
const char* mqttServer   = "broker.hivemq.com";     
const int   mqttPort     = 1883;     

const char* topicPerintah = "unsoed/tk245004/kelompokfathahnabil/perintah"; 
const int ledPin          = D6; // GPIO 12 (D6 pada NodeMCU ESP8266)

WiFiClient espClient; 
PubSubClient client(espClient); 

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk 
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;

  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  if (!doc["perintah"].is<const char*>()) {
    Serial.println("Format JSON tidak sesuai: key 'perintah' tidak ditemukan");
    return;
  }

  const char* perintah = doc["perintah"];

  if (strcmp(perintah, "ON") == 0) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Aktuator: ON");
  }
  else if (strcmp(perintah, "OFF") == 0) {
    digitalWrite(ledPin, LOW);
    Serial.println("Aktuator: OFF");
  }
  else {
    Serial.print("Perintah tidak dikenal: ");
    Serial.println(perintah);
  }
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
      Serial.println("berhasil terhubung!"); 
      client.subscribe(topicPerintah); // subscribe setelah berhasil terhubung 
      Serial.print("Subscribe ke topic: "); 
      Serial.println(topicPerintah); 
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
  
  hubungkanWiFi(); 
  client.setServer(mqttServer, mqttPort); 
  client.setCallback(callback); // daftarkan fungsi callback 
} 

void loop() { 
  if (!client.connected()) { 
    hubungkanMQTT(); 
  } 
  client.loop(); // memproses pesan MQTT masuk secara terus-menerus
}