#include <ESP8266WiFi.h>      // Library untuk menghubungkan ESP8266 ke Wi-Fi
#include <PubSubClient.h>     // Library untuk komunikasi MQTT
#include <ArduinoJson.h>      // Library untuk membuat data dalam format JSON
#include <math.h>             // Library matematika untuk fungsi sin()

// Konfigurasi Wi-Fi yang digunakan ESP8266
const char* ssid = "UdinPetot";           // Nama jaringan Wi-Fi
const char* password = "Admin1234";    // Password Wi-Fi

// Konfigurasi broker dan topic MQTT
const char* mqttServer = "broker.hivemq.com";               // Alamat broker MQTT
const int mqttPort = 1883;                                  // Port MQTT tanpa TLS
const char* topicData = "unsoed/tk245004/kelompok6/data";   // Topic pengiriman data

WiFiClient espClient;                  // Membuat koneksi jaringan untuk ESP8266
PubSubClient client(espClient);        // Membuat klien MQTT menggunakan koneksi tersebut

unsigned long jumlahBaca = 0;          // Menghitung jumlah pembacaan suhu

// Fungsi simulasi suhu sebagai pengganti sensor DHT22
float bacaSuhuMock() {
  jumlahBaca++; // Menambah jumlah pembacaan setiap fungsi dipanggil

  // Membuat suhu dasar yang berubah secara bertahap
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

  // Menambahkan variasi acak kecil pada suhu
  float noise = random(-5, 6) * 0.1;

  // Menggabungkan suhu dasar dan variasi acak
  float suhu = suhuDasar + noise;

  // Menambahkan lonjakan suhu 8 derajat setiap 25 pembacaan
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }

  return suhu; // Mengembalikan nilai suhu hasil simulasi
}

// Fungsi untuk menghubungkan ESP8266 ke Wi-Fi
void setupWiFi() {
  WiFi.mode(WIFI_STA);              // Mengatur ESP8266 sebagai perangkat klien
  WiFi.begin(ssid, password);       // Memulai koneksi ke jaringan Wi-Fi

  Serial.print("Menghubungkan Wi-Fi");

  // Menunggu sampai ESP8266 berhasil terhubung ke Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                     // Menunggu selama 500 milidetik
    Serial.print(".");              // Menampilkan tanda selama proses koneksi
  }

  Serial.println("\nWi-Fi berhasil terhubung!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());   // Menampilkan alamat IP ESP8266
}

// Fungsi untuk menghubungkan kembali ke broker MQTT jika koneksi terputus
void reconnectMQTT() {
  while (!client.connected()) {     // Mengulang selama MQTT belum terhubung

    // Membuat ID klien berdasarkan ID chip ESP8266
    String clientId = "ESP8266-Mock-";
    clientId += String(ESP.getChipId(), HEX);

    Serial.println("Mencoba koneksi MQTT...");

    // Mencoba menghubungkan klien ke broker MQTT
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT berhasil terhubung!");
    } else {
      // Menampilkan kode kesalahan jika koneksi gagal
      Serial.print("MQTT gagal, kode: ");
      Serial.println(client.state());
      delay(5000); // Menunggu 5 detik sebelum mencoba kembali
    }
  }
}

void setup() {
  Serial.begin(115200);            // Memulai komunikasi Serial Monitor
  delay(1000);                     // Memberikan jeda awal 1 detik
  randomSeed(micros());            // Mengatur nilai awal untuk bilangan acak
  setupWiFi();                     // Menghubungkan ESP8266 ke Wi-Fi
  client.setServer(mqttServer, mqttPort); // Menentukan broker dan port MQTT
  reconnectMQTT();                 // Menghubungkan ESP8266 ke broker MQTT
}

void loop() {
  // Memeriksa dan menghubungkan kembali Wi-Fi jika terputus
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }

  // Memeriksa dan menghubungkan kembali MQTT jika terputus
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop(); // Menjaga komunikasi MQTT tetap berjalan

  float suhu = bacaSuhuMock(); // Mengambil nilai suhu hasil simulasi

  // Membuat dokumen JSON untuk menyimpan nilai suhu
  StaticJsonDocument<128> doc;
  doc["suhu"] = suhu;          // Menyimpan suhu pada atribut bernama "suhu"

  char payload[128];           // Menyediakan buffer untuk pesan JSON
  serializeJson(doc, payload); // Mengubah dokumen JSON menjadi teks

  // Mengirim data suhu ke topic MQTT
  bool berhasil = client.publish(topicData, payload);

  // Menampilkan informasi pembacaan dan pengiriman pada Serial Monitor
  Serial.print("Pembacaan ke-");
  Serial.print(jumlahBaca);
  Serial.print(" | Suhu mock: ");
  Serial.print(suhu, 2);       // Menampilkan suhu dengan 2 angka desimal
  Serial.print(" °C | Topic: ");
  Serial.print(topicData);
  Serial.print(" | Publish: ");
  Serial.println(berhasil ? "Berhasil" : "Gagal");

  delay(5000); // Mengirim pembacaan berikutnya setelah 5 detik
}