# Pertemuan 3 — Protokol Komunikasi IoT (HTTP & MQTT dengan Format JSON)

## 1. Deskripsi Singkat Percobaan
Praktikum ini terdiri dari dua percobaan yang membahas dua protokol komunikasi IoT
yang paling umum digunakan pada ESP8266/ESP32:

- **Percobaan 3A** — mengirim data sensor (suhu & kelembaban) dari mikrokontroler ke
  server `httpbin.org/post` menggunakan protokol **HTTP** (metode POST) dalam format JSON.
- **Percobaan 3B** — mempublikasikan data sensor yang sama ke broker MQTT publik
  `broker.hivemq.com` menggunakan pola **publish-subscribe**, diverifikasi menggunakan
  aplikasi **MQTT Explorer** sebagai subscriber.

Kedua data dikemas dalam format **JSON** menggunakan pustaka **ArduinoJson**, sehingga
mudah dipertukarkan antar platform.

## 2. Skematik / Alur Rangkaian
Board yang digunakan adalah **ESP8266** (bukan ESP32), sehingga library yang dipakai
adalah `ESP8266WiFi.h` dan `ESP8266HTTPClient.h`. Rangkaian tidak memerlukan sensor
fisik tambahan (data suhu & kelembaban masih berupa nilai contoh/hardcoded), sehingga
skematik hanya berupa:

```
ESP8266 DevKit ---(USB)--- Laptop/PC (Arduino IDE, Serial Monitor)
ESP8266 DevKit ---(WiFi)--- Internet ---(HTTPS)--- httpbin.org/post   (3A)
ESP8266 DevKit ---(WiFi)--- Internet ---(TCP:1883)--- broker.hivemq.com (3B)
```

> Tambahkan foto proses perangkaian/praktikum Anda di sini, contoh:
> `![Foto praktikum](foto-praktikum.jpg)`

> Tambahkan GIF/video demonstrasi program berjalan di sini jika diperlukan, contoh:
> `![Demo](demo.gif)`

---

## 3. Percobaan 3A — Komunikasi HTTP (`code/Percobaan_3A_HTTP.ino`)

### 3.1 Library / Dependencies
| Library | Fungsi |
|---|---|
| `ESP8266WiFi.h` | Menghubungkan ESP8266 ke jaringan WiFi |
| `ESP8266HTTPClient.h` | Membuat request HTTP (GET/POST) dari ESP8266 ke server |
| `ArduinoJson.h` (by Benoit Blanchon) | Membuat dan mengubah (serialize) objek data menjadi format JSON |

Karena endpoint tujuan (`httpbin.org`) menggunakan **HTTPS**, program juga
memanfaatkan `WiFiClientSecure` sebagai client koneksi terenkripsi.

### 3.2 Penjelasan Code Secara Detail

**`setup()`**
- `Serial.begin(115200)` — mengaktifkan komunikasi serial dengan baud rate 115200 agar
  proses debugging bisa dipantau lewat Serial Monitor.
- `WiFi.begin(ssid, password)` — memulai proses koneksi ke jaringan WiFi menggunakan
  SSID dan password yang telah ditentukan.
- `while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }` —
  **loop (percabangan berulang)** yang menahan program hingga status WiFi benar-benar
  `WL_CONNECTED`. Setiap 500 ms program mencetak tanda titik sebagai indikator visual
  bahwa proses koneksi sedang berlangsung.

**`loop()`**
- `if (WiFi.status() == WL_CONNECTED) { ... }` — **conditional** utama: seluruh proses
  pengiriman data hanya dijalankan apabila ESP8266 masih terhubung ke WiFi. Ini mencegah
  program mencoba mengirim data saat tidak ada koneksi jaringan.
- `WiFiClientSecure client; client.setInsecure();` — membuat client koneksi aman (TLS)
  tanpa validasi sertifikat SSL (disederhanakan untuk keperluan praktikum).
- `http.begin(client, serverUrl); http.addHeader("Content-Type", "application/json");`
  — menyiapkan request HTTP ke `serverUrl` dan menambahkan header yang memberi tahu
  server bahwa body request berformat JSON.
- `JsonDocument doc; doc["suhu"] = 28.5; doc["kelembaban"] = 65.0;` — membuat objek JSON
  berisi dua pasangan key-value data sensor (nilai masih berupa data contoh/statis).
- `serializeJson(doc, requestBody);` — mengubah objek `doc` menjadi bentuk teks (String)
  agar bisa dikirim sebagai body HTTP.
- `int httpResponseCode = http.POST(requestBody);` — mengirim data melalui metode
  HTTP POST dan menyimpan kode status response dari server ke variabel
  `httpResponseCode`.
- `if (httpResponseCode > 0) { ... } else { ... }` — **conditional**: jika nilai lebih
  besar dari 0 berarti request berhasil terkirim dan server merespons (kode response
  ditampilkan beserta isi body-nya via `http.getString()`); jika tidak (≤0), berarti
  terjadi kegagalan koneksi/pengiriman sehingga kode error ditampilkan.
- `http.end();` — menutup koneksi HTTP untuk membebaskan resource.
- `delay(10000);` — menjeda program 10 detik sebelum data sensor berikutnya dikirim.

### 3.3 Jawaban Pertanyaan Praktikum Terkait Code
1. **Diagram alur** proses HTTP POST: Mulai → Setup WiFi → Cek status WiFi → Buat objek
   JSON → Serialize ke String → `http.begin` + `addHeader` → `http.POST()` → Cek
   response code (`>0` → tampilkan code & body; `≤0` → tampilkan pesan gagal) →
   `http.end()` → delay 10 detik → kembali ke awal loop.
2. **`http.addHeader("Content-Type", "application/json")`** memberi tahu server bahwa
   body yang dikirim berformat JSON, sehingga server dapat mem-parsing data dengan
   benar sebagai objek JSON.
3. **Kode response 200** berarti *OK* — request berhasil diproses dan server
   mengembalikan data sesuai permintaan. Contoh lain: **404 Not Found**, artinya
   endpoint/URL yang dituju tidak ditemukan di server.
4. **Modifikasi penambahan waktu (`millis()`)** — ditambahkan satu baris:
   ```cpp
   doc["waktu_ms"] = millis(); // waktu sejak ESP8266 dinyalakan (ms)
   ```
   diletakkan setelah `doc["kelembaban"]` dan sebelum `serializeJson()`, sehingga field
   `waktu_ms` ikut terbawa saat objek JSON diserialisasi menjadi `requestBody`. Hasil
   JSON menjadi: `{"suhu":28.5,"kelembaban":65.0,"waktu_ms":123456}`.

---

## 4. Percobaan 3B — Komunikasi MQTT (`code/Percobaan_3B_MQTT.ino`)

### 4.1 Library / Dependencies
| Library | Fungsi |
|---|---|
| `ESP8266WiFi.h` | Menghubungkan ESP8266 ke jaringan WiFi |
| `PubSubClient.h` (by Nick O'Leary) | Implementasi protokol MQTT (connect, publish, loop) |
| `ArduinoJson.h` | Membuat dan mengubah data sensor menjadi format JSON |

### 4.2 Penjelasan Code Secara Detail

**Variabel global**
- `mqttServer = "broker.hivemq.com"`, `mqttPort = 1883` — alamat dan port broker MQTT
  publik yang digunakan.
- `mqttTopic = "unsoed/tk245004/kelompokfathahnabil/sensor"` — topic unik tempat data
  dipublikasikan, agar tidak tercampur dengan data kelompok/pengguna lain pada broker
  publik yang sama.
- `WiFiClient espClient; PubSubClient client(espClient);` — objek client MQTT yang
  dibangun di atas koneksi WiFi (`espClient`).

**`hubungkanWiFi()`**
- Fungsi terpisah untuk menghubungkan ESP8266 ke WiFi (sama seperti pada 3A), dipanggil
  sekali di `setup()`.

**`hubungkanMQTT()`**
- `while (!client.connected()) { ... }` — **loop conditional**: mengulang proses koneksi
  ke broker selama status client belum terhubung.
- `String clientId = "ESP8266Client-" + String(random(0xffff), HEX);` — membuat ID
  client acak (dalam heksadesimal) agar setiap perangkat memiliki identitas unik saat
  terhubung ke broker (mencegah bentrok ID antar client).
- `if (client.connect(clientId.c_str())) { ... } else { ... }` — **conditional**: jika
  koneksi ke broker berhasil, tampilkan pesan berhasil; jika gagal, tampilkan kode error
  (`client.state()`) dan tunggu 2 detik sebelum mencoba lagi.

**`setup()`**
- Memanggil `hubungkanWiFi()`, lalu `client.setServer(mqttServer, mqttPort)` untuk
  mengatur alamat broker yang akan digunakan.

**`loop()`**
- `if (!client.connected()) { hubungkanMQTT(); }` — **conditional**: memastikan koneksi
  ke broker selalu aktif; jika terputus, program otomatis mencoba menyambung ulang.
- `client.loop();` — memproses pesan MQTT masuk/keluar dan menjaga koneksi tetap hidup
  (mengirim keep-alive/ping ke broker). Fungsi ini **wajib** dipanggil setiap iterasi
  agar koneksi tidak timeout.
- `JsonDocument doc; doc["suhu"] = 28.5; doc["kelembaban"] = 65.0;` — membuat objek data
  sensor dalam format JSON (sama seperti pada 3A).
- `char buffer[128]; serializeJson(doc, buffer);` — mengubah objek JSON menjadi array
  karakter (C-string) yang dibutuhkan oleh fungsi `client.publish()`.
- `client.publish(mqttTopic, buffer);` — mempublikasikan data (payload JSON) ke topic
  MQTT yang telah ditentukan.
- `delay(5000);` — menjeda 5 detik sebelum publish data berikutnya.

### 4.3 Jawaban Pertanyaan Praktikum Terkait Code
1. **Fungsi topic & alasan harus unik** — topic adalah "alamat"/kanal pengelompokan
   data pada broker; subscriber hanya menerima data dari topic yang di-subscribe. Topic
   dibuat unik (menyertakan nama kelompok) agar data tidak tercampur dengan kelompok
   lain pada broker publik yang sama.
2. **Fungsi `client.loop()`** — memproses pesan masuk, menjaga koneksi tetap hidup
   (keep-alive), dan harus dipanggil terus-menerus di dalam `loop()` agar koneksi MQTT
   tidak terputus.
3. **Jika koneksi ke broker terputus** — kondisi `if (!client.connected())` akan
   mendeteksinya, lalu `hubungkanMQTT()` otomatis mencoba menyambungkan ulang sebelum
   proses publish data dilanjutkan; selama proses reconnect, data sensor tidak terkirim.

---

## 5. Analisis Perbandingan HTTP vs MQTT
| Aspek | HTTP | MQTT |
|---|---|---|
| Model | Request-response (stateless) | Publish-subscribe |
| Overhead header | Besar (header lengkap tiap request) | Kecil (hanya topic + payload) |
| Koneksi | Dibuka & ditutup tiap request | Persistent (tetap terbuka) |
| Cocok untuk | Pengiriman periodik / tidak terlalu sering | Pengiriman kontinu / sering, hemat daya |

**Kesimpulan:** untuk pengiriman data sensor terus-menerus dalam jangka waktu lama,
**MQTT** lebih sesuai karena overhead lebih kecil dan koneksinya persistent, sehingga
lebih hemat daya dan bandwidth dibanding HTTP.

## 6. Peran Format JSON
JSON dipilih karena strukturnya ringan, berbasis teks, dan mudah dibaca baik oleh
manusia maupun mesin. Format key-value ini didukung oleh hampir semua bahasa
pemrograman dan platform (web, mobile, cloud, database), sehingga data dari perangkat
IoT yang berbeda dapat saling dipertukarkan dan diproses oleh berbagai sistem tanpa
konversi format yang rumit — mendukung interoperabilitas antar platform.

