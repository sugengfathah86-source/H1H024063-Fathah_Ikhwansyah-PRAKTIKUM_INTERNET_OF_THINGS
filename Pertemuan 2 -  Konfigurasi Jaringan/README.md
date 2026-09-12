# Pertemuan 2 — Konfigurasi Jaringan (WiFi Mode Station & Access Point)

**Mata Kuliah:** Praktikum Sistem Internet of Things (TK245005)
**Board:** ESP8266 (`ESP8266WiFi.h`)
**Praktikan:** Fathah Ikhwansyah / H1H024063

## Deskripsi Singkat Percobaan

Pertemuan ini membahas dua percobaan konfigurasi jaringan WiFi pada ESP8266:

- **Percobaan 2A** — ESP8266 dikonfigurasi sebagai **Station (STA)**, menyambung ke jaringan WiFi yang sudah ada. Code final ditambahkan mekanisme *auto-reconnect* non-blocking, karena code dasar dari modul (`while (WiFi.status() != WL_CONNECTED)` tanpa timeout) akan macet total apabila SSID/password salah.
- **Percobaan 2B** — ESP8266 dikonfigurasi sebagai **Access Point (AP)** mandiri dengan SSID `UdinPetot`. Code final dikembangkan menjadi mode **AP+STA**, sehingga ESP8266 tetap menyediakan AP sendiri sekaligus tersambung ke WiFi rumah — relevan untuk skenario provisioning perangkat IoT.

## Library / Dependencies

| Library | Fungsi |
|---|---|
| `ESP8266WiFi.h` | Bawaan Arduino core untuk ESP8266; menyediakan seluruh fungsi WiFi yang dipakai (`WiFi.mode()`, `WiFi.begin()`, `WiFi.softAP()`, `WiFi.status()`, dll). Tidak perlu diinstal terpisah — otomatis tersedia setelah board manager ESP8266 terpasang di Arduino IDE. |

Tidak ada library eksternal tambahan (mis. dari Library Manager) yang digunakan pada kedua percobaan ini.

---

## Percobaan 2A — Mode Station (STA) dengan Auto-Reconnect

### Code final

```cpp
#include <ESP8266WiFi.h>   // sesuai board kamu: ESP8266

const char* sta_ssid     = "NAMA_WIFI_ANDA";     // ganti dengan SSID WiFi rumah
const char* sta_password = "PASSWORD_WIFI_ANDA"; // ganti dengan password WiFi rumah

// ==== Tambahan untuk fitur auto-reconnect ====
unsigned long previousMillis = 0;          // menyimpan waktu pengecekan terakhir
const long reconnectInterval = 5000;       // interval cek/reconnect (5 detik)

void connectWiFi() {
  Serial.print("Menghubungkan ke WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid, sta_password);

  unsigned long startAttempt = millis();
  // Batasi percobaan awal maksimal 10 detik agar program tidak macet selamanya
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println();
    Serial.println("Gagal terhubung, akan dicoba lagi secara berkala.");
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();   // percobaan koneksi pertama kali
}

void loop() {
  unsigned long currentMillis = millis();

  // Non-blocking: hanya cek/reconnect setiap `reconnectInterval` ms,
  // tanpa delay() panjang yang menghentikan program.
  if (currentMillis - previousMillis >= reconnectInterval) {
    previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Status: Terhubung");
    } else {
      Serial.println("Status: Terputus, mencoba menghubungkan ulang...");

      // Memulai ulang koneksi WiFi (reconnect otomatis)
      WiFi.disconnect();
      WiFi.begin(sta_ssid, sta_password);
    }
  }
}
```

### Penjelasan fungsi

| Fungsi | Penjelasan |
|---|---|
| `connectWiFi()` | Membungkus seluruh proses koneksi awal: mengatur mode STA, memulai `WiFi.begin()`, menunggu hingga terhubung (maksimal 10 detik), lalu mencetak hasilnya. Dipisah menjadi fungsi sendiri agar bisa dipanggil ulang tanpa duplikasi kode. |
| `setup()` | Hanya memanggil `Serial.begin()` dan `connectWiFi()` sekali di awal program. |
| `loop()` | Menjalankan pengecekan status koneksi secara berkala (non-blocking) dan memicu reconnect otomatis bila terputus. |

### Penjelasan percabangan (conditional)

| Kondisi | Penjelasan |
|---|---|
| `while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000)` | Loop menunggu koneksi berhasil, tetapi dibatasi 10 detik — dua syarat harus sama-sama benar (`&&`) agar loop terus berjalan: belum terhubung **dan** belum melewati batas waktu. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` (dalam `connectWiFi()`) | Menentukan pesan/data yang dicetak setelah proses tunggu selesai: berhasil (tampilkan IP/MAC/RSSI) atau gagal (pesan gagal). |
| `if (currentMillis - previousMillis >= reconnectInterval) { ... }` | Percabangan berbasis waktu (non-blocking timer) — isi blok hanya dijalankan tiap 5 detik, menggantikan `delay(5000)` yang blocking. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { WiFi.disconnect(); WiFi.begin(...); }` (dalam `loop()`) | Inti auto-reconnect: jika masih terhubung, tidak melakukan apa-apa selain mencetak status; jika terputus, memutus koneksi lama lalu mencoba menyambung ulang. |

---

## Percobaan 2B — Mode AP+STA

### Code final

```cpp
#include <ESP8266WiFi.h>   // sesuai board kamu: ESP8266
// --- Kredensial untuk mode Access Point (sama seperti code Percobaan 2B asli) ---
const char* ap_ssid     = "UdinPetot";
const char* ap_password = "Admin1234"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);

  // 1. Set mode gabungan: Access Point + Station berjalan bersamaan
  WiFi.mode(WIFI_AP_STA);

  // 2. Aktifkan Access Point terlebih dahulu (persis seperti code Percobaan 2B)
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point aktif!");
  Serial.print("AP SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("AP IP Address : ");
  Serial.println(WiFi.softAPIP());   // biasanya 192.168.4.1

  // 3. Mulai koneksi Station ke jaringan WiFi yang sudah ada
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi rumah");

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Station berhasil terhubung ke WiFi rumah!");
    Serial.print("STA IP Address : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Station gagal terhubung (AP tetap aktif).");
  }
}

void loop() {
  // Tampilkan status kedua peran (AP dan STA) setiap 5 detik
  Serial.println("---------------------------------");

  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("[AP]  Jumlah perangkat terhubung ke AP : ");
  Serial.println(jumlahClient);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[STA] Status ke WiFi rumah : Terhubung, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[STA] Status ke WiFi rumah : Terputus");
    // Opsional: coba reconnect STA tanpa mengganggu AP yang sedang aktif
    WiFi.begin(sta_ssid, sta_password);
  }

  delay(5000);
}
```

### Penjelasan fungsi

| Fungsi | Penjelasan |
|---|---|
| `setup()` | Mengaktifkan mode `WIFI_AP_STA`, menyalakan Access Point (`softAP`), lalu memulai proses koneksi Station ke WiFi rumah dengan batas waktu tunggu 10 detik. |
| `loop()` | Setiap 5 detik, mencetak jumlah client yang terhubung ke AP dan status koneksi Station; jika Station terputus, mencoba menyambung ulang tanpa mematikan AP. |

### Penjelasan percabangan (conditional)

| Kondisi | Penjelasan |
|---|---|
| `while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000)` | Sama seperti Percobaan 2A — menunggu Station terhubung, dibatasi 10 detik agar AP tidak ikut macet jika Station gagal. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` (di `setup()`) | Melaporkan hasil percobaan koneksi Station setelah proses tunggu: berhasil (tampilkan IP) atau gagal (AP tetap aktif, hanya Station yang tidak tersambung). |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { WiFi.begin(sta_ssid, sta_password); }` (di `loop()`) | Jika Station terhubung, hanya cetak status dan IP; jika terputus, coba sambung ulang tanpa mengganggu AP yang sedang berjalan. |

### Ringkasan perubahan dari code Percobaan 2B asli

- `WiFi.mode(WIFI_AP)` → `WiFi.mode(WIFI_AP_STA)`
- Ditambahkan kredensial dan proses koneksi Station (`sta_ssid`, `sta_password`, `WiFi.begin()`, pengecekan status dengan timeout)
- Di `loop()`, ditambahkan pemantauan status Station berdampingan dengan pemantauan jumlah client AP yang sudah ada sebelumnya

---

## Jawaban Pertanyaan Praktikum (terkait code)

**Apa fungsi `WiFi.mode(WIFI_STA)` / `WiFi.mode(WIFI_AP_STA)`?**
Menentukan peran modul WiFi sebelum `WiFi.begin()`/`WiFi.softAP()` dipanggil. `WIFI_STA` mengaktifkan hanya peran klien (Station); `WIFI_AP_STA` mengaktifkan peran Access Point dan Station secara bersamaan, karena masing-masing mode mengarahkan driver WiFi menjalankan stack proses yang berbeda.

**Apa yang terjadi jika SSID/password salah?**
`WiFi.status()` tidak akan pernah bernilai `WL_CONNECTED`. Pada code dasar dari modul yang memakai `while (WiFi.status() != WL_CONNECTED)` tanpa batas waktu, program akan macet total (infinite loop) mencetak titik (".") terus-menerus. Karena itu code final di atas menambahkan timeout 10 detik pada proses tunggu.

**Mengapa IP default Access Point 192.168.4.1?**
Nilai bawaan dari SDK Espressif untuk mode soft-AP, menggunakan subnet privat 192.168.4.0/24 — dipilih berbeda dari rentang default router rumah (192.168.0.x/192.168.1.x) agar tidak bentrok saat mode AP+STA aktif bersamaan.

**Perbedaan mendasar STA vs AP?**
STA: ESP8266 sebagai klien yang menyambung ke jaringan yang sudah ada, mendapat IP dari DHCP router. AP: ESP8266 sebagai penyedia jaringan sendiri, menjalankan DHCP server internal untuk klien yang terhubung ke SSID `UdinPetot`.

**Risiko jika password AP kosong/terlalu sederhana?**
Siapa pun dalam jangkauan sinyal bisa terhubung tanpa otorisasi dan berpotensi mengakses halaman konfigurasi ESP8266, rentan brute-force, serta rawan sniffing/manipulasi data pada jaringan AP. Disarankan password acak minimal 8–12 karakter kombinasi huruf/angka/simbol.

---

## Skematik / Diagram Rangkaian

Kedua percobaan hanya menggunakan board ESP8266 yang tersambung ke komputer melalui kabel USB (tanpa komponen tambahan seperti LED indikator). Tidak ada rangkaian eksternal yang perlu digambar.

## Dokumentasi
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/a0cc30d8-ee19-454a-aaeb-4c0ee331c004" />

- **GIF/video demonstrasi Serial Monitor:** _(tempel/lampirkan tautan video atau GIF hasil Serial Monitor saat program berjalan di sini)_

> Catatan: ganti nilai `sta_ssid` dan `sta_password` pada kedua file kode sesuai jaringan WiFi rumah/hotspot yang digunakan saat praktikum, sebelum di-upload ke board.
