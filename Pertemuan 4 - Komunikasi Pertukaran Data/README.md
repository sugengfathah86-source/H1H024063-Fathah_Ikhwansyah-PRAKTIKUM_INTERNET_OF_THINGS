# Modul 4: Komunikasi Pertukaran Data Dua Arah (Publish dan Subscribe)

**Nama:** Fathah Ikhwansyah  
**NIM:** H1H024063

## 📖 Detail Percobaan
Pada modul ini, dilakukan dua percobaan utama untuk membangun komunikasi IoT dua arah (*Full Duplex*) berbasis protokol MQTT menggunakan broker `broker.hivemq.com`:
1. **Percobaan 4A (Subscribe & Deserialisasi JSON):** Menggunakan ESP8266 secara pasif untuk menerima data berformat JSON dari server/klien lain dan mengekstrak perintah di dalamnya untuk menyalakan atau mematikan LED (Aktuator).
2. **Percobaan 4B (Pertukaran Dua Arah Simultannya):** Menambahkan fungsi *publish* ke program sebelumnya. ESP8266 secara terus-menerus mengirimkan data suhu dari sensor DHT11 tanpa menghalangi (blocking) fungsi *subscribe* yang mendengarkan perintah kendali masuk.

---

## 🛠️ Library dan Dependencies
Untuk menjalankan kode program ini, pastikan *dependencies* berikut telah terinstal pada Arduino IDE:
* `ESP8266WiFi.h` : Library bawaan core ESP8266 untuk fitur konektivitas jaringan Wi-Fi. (Catatan: Terjadi penyesuaian hardware dari modul yang menyarankan ESP32 menjadi ESP8266).
* `PubSubClient.h` : Digunakan untuk membangun koneksi ke broker MQTT, melakukan *publish* data, dan menerima pesan melalui *subscribe*.
* `ArduinoJson.h` : Digunakan untuk melakukan serialisasi (membentuk data JSON sebelum dikirim) dan deserialisasi (memecah data JSON yang diterima).
* `DHT.h` : Library sensor untuk membaca suhu dan kelembapan dari DHT11/DHT22.

---

## 🔌 Skematik dan Rangkaian (Penyesuaian Pin ESP8266)
Dikarenakan ada penyesuaian penggunaan mikrokontroler dari ESP32 menjadi NodeMCU ESP8266, berikut adalah konfigurasi pin *final* yang digunakan saat praktikum:
* **Sensor Suhu (DHT11):** Pin Data (OUT) terhubung ke pin **D2 / GPIO 4** ESP8266.
* **Aktuator (LED):** Anoda (Kaki Panjang) terhubung melalui resistor 220 Ohm ke pin **D6 / GPIO 12** ESP8266. Katoda (Kaki Pendek) ke pin **GND**.

*(Catatan untuk Praktikan: Ubah teks di bawah ini dengan gambar/foto asli rangkaian Anda)*
`![Foto Rangkaian Praktikum](link_foto_rangkaian_anda_disini.jpg)`

---

## 💻 Penjelasan Code & Fungsi (Percobaan 4B - Full Duplex)

Berikut adalah rincian fungsionalitas dari program utama (`percobaan2.ino`):

### 1. Deklarasi & Setup
* `WiFiClient espClient; PubSubClient client(espClient);` : Inisialisasi antarmuka klien jaringan Wi-Fi dan klien MQTT.
* `void setup()` : Dijalankan satu kali saat perangkat menyala. Berisi pengaturan *baud rate* `Serial.begin(115200)`, deklarasi pin mode `pinMode(ledPin, OUTPUT)`, inisialisasi sensor `dht.begin()`, dan konfigurasi awal MQTT via `client.setServer()` dan `client.setCallback()`.

### 2. Fungsi Konektivitas
* `void hubungkanWiFi()` : Melakukan perulangan `while` yang terus menahan (mem-blokir) program dan mencetak "." selama ESP belum terkoneksi ke *Access Point*.
* `void hubungkanMQTT()` : Mengenerate `clientId` secara acak. Jika koneksi sukses, perintah **`client.subscribe(topicPerintah)`** dipanggil. 
  *(Pertanyaan Praktikum Terjawab: Pemanggilan fungsi subscribe diletakkan di sini, bukan di setup(), agar ESP secara otomatis mendaftar ulang/berlangganan ke topik ketika terjadi rekoneksi jaringan MQTT akibat putus sinyal).*

### 3. Fungsi Non-Blocking (Pengganti `delay()`) di `loop()`
Sistem *Full-Duplex* melarang penggunaan `delay()` karena memblokir penerimaan pesan.
```cpp
if (millis() - waktuTerakhirPublish > intervalPublish) { 
  waktuTerakhirPublish = millis(); 
  // Blok pengiriman (publish) data DHT11 dijalankan tiap 5 detik
}
```
Penjelasan Logika: millis() adalah waktu berjalan sejak alat menyala. Program mengecek secara asinkron; jika selisih waktu saat ini dengan proses publish terakhir belum mencapai interval 5 detik, program mengabaikan blok publish dan langsung menuju fungsi client.loop() di bawahnya agar pesan subscribe selalu direspons seketika.

4. Fungsi Penerimaan dan Percabangan (callback)
Berjalan otomatis layaknya sebuah "interupsi" saat ESP menerima payload pesan dari MQTT broker.
```
void callback(char* topic, byte* payload, unsigned int length) { 
  // ... loop per byte payload ...
  JsonDocument doc; 
  if (deserializeJson(doc, pesan)) return; 
  const char* perintah = doc["perintah"]; 
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW); 
}
```
Penjelasan Percabangan/Kondisional:

if (deserializeJson(doc, pesan)) return; : Kondisional proteksi. Jika format pesan rusak (bukan JSON), fungsi deserialize mengembalikan error/true, dan perintah return akan menghentikan seluruh fungsi callback secara paksa untuk menghindari sistem crash.

String(perintah) == "ON" ? HIGH : LOW : Sebuah Ternary Operator (percabangan If-Else singkat). Jika isi variabel perintah sama persis dengan teks "ON", pin LED mendapat arus HIGH. Jika tidak, diberi nilai LOW.

📝 Modifikasi Kode (Jawaban Tugas Praktikum 4.5.4 & 4.6.4)

A. Modifikasi Intensitas LED (PWM) - Modul 4A
Tugas: Menambahkan nilai intensitas dari pesan JSON untuk kecerahan LED menggunakan PWM.
```
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

  // === MODIFIKASI: ambil nilai "intensitas" (0-255) dengan fallback 255 ===
  // Operator "|" pada ArduinoJson berarti: pakai nilai dari JSON jika ada,
  // atau pakai nilai default (255 = terang penuh) jika key "intensitas" tidak dikirim.
  // Ini menjaga kompatibilitas dengan pesan lama yang hanya berisi {"perintah": "..."}.
  int intensitas = doc["intensitas"] | 255;

  // Batasi (clamp) nilai agar selalu berada pada rentang valid 0-255,
  // supaya nilai yang salah/ekstrem dari client tidak merusak PWM.
  if (intensitas < 0)   intensitas = 0;
  if (intensitas > 255) intensitas = 255;

  if (strcmp(perintah, "ON") == 0) {
    // MODIFIKASI: analogWrite menggantikan digitalWrite agar kecerahan
    // LED bisa diatur bertingkat (PWM), bukan hanya ON/OFF biner.
    analogWrite(ledPin, intensitas);
    Serial.print("Aktuator: ON, intensitas=");
    Serial.println(intensitas);
  }
  else if (strcmp(perintah, "OFF") == 0) {
    // OFF tetap berarti LED mati total, terlepas dari nilai intensitas yang dikirim.
    analogWrite(ledPin, 0);
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

  // MODIFIKASI: samakan rentang PWM ESP8266 (default 0-1023) menjadi 0-255
  // agar nilai "intensitas" dari JSON (0-255) bisa dipakai langsung tanpa konversi.
  analogWriteRange(255);
  analogWrite(ledPin, 0);

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

```
B. Modifikasi Kendali Aktuator Ganda (Buzzer) - Modul 4B
Tugas: Menambahkan topik perintah baru untuk aktuator kedua dan memisahkan alur logika.
```
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
// MODIFIKASI: topic baru khusus untuk mengendalikan aktuator kedua (buzzer)
const char* topicBuzzer   = "unsoed/tk245004/kelompokfathahnabil/perintah_buzzer";

#define DHTPIN 4          // Pin D2 pada NodeMCU ESP8266 (GPIO 4)
#define DHTTYPE DHT11     // Sensor DHT11

const int ledPin    = D6;  // Pin D6 pada NodeMCU ESP8266 (GPIO 12) - LED
const int buzzerPin  = D7; // MODIFIKASI: Pin D7 pada NodeMCU ESP8266 (GPIO 13) - Buzzer

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
  bool nyala = (String(perintah) == "ON");

  // MODIFIKASI: bandingkan parameter "topic" (bukan hanya isi pesannya) untuk
  // menentukan aktuator mana yang dituju oleh perintah yang baru diterima.
  if (strcmp(topic, topicPerintah) == 0) {
    digitalWrite(ledPin, nyala ? HIGH : LOW);
    Serial.print("Perintah diterima [LED] -> Aktuator: ");
    Serial.println(perintah);
  }
  else if (strcmp(topic, topicBuzzer) == 0) {
    digitalWrite(buzzerPin, nyala ? HIGH : LOW);
    Serial.print("Perintah diterima [BUZZER] -> Aktuator: ");
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
      client.subscribe(topicPerintah);
      // MODIFIKASI: subscribe topic kedua agar perintah buzzer juga diterima
      client.subscribe(topicBuzzer);
      Serial.println("Terhubung dan subscribe topic perintah & perintah_buzzer");
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
  pinMode(buzzerPin, OUTPUT); // MODIFIKASI: inisialisasi pin buzzer sebagai output
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

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

```
## 💻 Dokumentasi
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/b5ec55f5-2f88-4053-b889-08237ee32127" />
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/126305d6-4421-4312-a99d-36cb16f0be76" />

