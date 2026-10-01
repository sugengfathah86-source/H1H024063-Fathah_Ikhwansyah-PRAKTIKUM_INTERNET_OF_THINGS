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
// Penambahan pada blok fungsi callback()
const char* perintah = doc["perintah"];
int intensitas = doc["intensitas"]; // Mengambil data angka intensitas (contoh JSON: {"perintah":"ON", "intensitas":200})

if (String(perintah) == "ON") {
  analogWrite(ledPin, intensitas); // Memberikan sinyal PWM sesuai angka JSON (0-255)
  Serial.println("Aktuator LED: ON Menyala Redup/Terang");
} else if (String(perintah) == "OFF") {
  analogWrite(ledPin, 0);          // Mematikan LED (PWM 0)
}
```
B. Modifikasi Kendali Aktuator Ganda (Buzzer) - Modul 4B
Tugas: Menambahkan topik perintah baru untuk aktuator kedua dan memisahkan alur logika.
```
// 1. Deklarasi di bagian awal
const char* topicPerintah = "unsoed/tk245004/kelompokfathahnabil/perintah"; 
const char* topicBuzzer   = "unsoed/tk245004/kelompokfathahnabil/buzzer"; 
const int buzzerPin = D7; // Deklarasi pin baru untuk aktuator buzzer

// 2. Modifikasi di fungsi hubungkanMQTT()
client.subscribe(topicPerintah);
client.subscribe(topicBuzzer); // Daftar mendengarkan topik kedua

// 3. Percabangan Topik di fungsi callback()
if (String(topic) == topicPerintah) {
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
  Serial.println("Perintah LED diterima dan dieksekusi!");
} 
else if (String(topic) == topicBuzzer) {
  digitalWrite(buzzerPin, String(perintah) == "ON" ? HIGH : LOW);
  Serial.println("Perintah Buzzer diterima dan dieksekusi!");
}
```
