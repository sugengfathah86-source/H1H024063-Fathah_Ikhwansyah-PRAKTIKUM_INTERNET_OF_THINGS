# README — Modifikasi Program Percobaan 2A & 2B (Pertanyaan No. 4)

Board: ESP8266 (menggunakan `ESP8266WiFi.h`)

Dokumen ini berisi jawaban untuk:
- **Percobaan 2A** — modifikasi agar ESP8266 melakukan *reconnect* otomatis saat WiFi terputus
- **Percobaan 2B** — modifikasi agar ESP8266 berjalan pada mode **AP+STA** (Access Point tetap aktif sekaligus tersambung ke WiFi rumah)

---

## 1. Percobaan 2A — Auto-Reconnect (Mode Station)

### Kode program lengkap

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

### Penjelasan baris/bagian kode

| Kode | Penjelasan |
|---|---|
| `#include <ESP8266WiFi.h>` | Pustaka WiFi khusus board ESP8266 (pengganti `WiFi.h` pada ESP32). Fungsi seperti `WiFi.mode()`, `WiFi.begin()`, `WiFi.status()` tetap tersedia dengan nama yang sama. |
| `unsigned long previousMillis = 0;` | Menyimpan waktu (ms) terakhir kali program melakukan pengecekan status WiFi. Pengganti `delay()` panjang agar program tidak berhenti total. |
| `const long reconnectInterval = 5000;` | Interval pengecekan/reconnect: setiap 5000 ms (5 detik). |
| `void connectWiFi() { ... }` | Fungsi terpisah yang membungkus proses koneksi awal (`WiFi.mode`, `WiFi.begin`, pengecekan status), sehingga bisa dipanggil ulang tanpa menduplikasi kode. |
| `unsigned long startAttempt = millis();` + `while (... && millis() - startAttempt < 10000)` | Membatasi percobaan koneksi awal maksimal 10 detik, mencegah program macet selamanya (*infinite loop*) jika WiFi tidak tersedia. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` (dalam `connectWiFi()`) | Setelah timeout, cek apakah berhasil terhubung: cetak IP/MAC/RSSI jika berhasil, atau pesan gagal jika tidak. |
| `unsigned long currentMillis = millis();` (di `loop()`) | Mengambil waktu saat ini setiap kali `loop()` berjalan, untuk dibandingkan dengan `previousMillis`. |
| `if (currentMillis - previousMillis >= reconnectInterval) { previousMillis = currentMillis; ... }` | Pola *non-blocking timer* — blok di dalamnya hanya dijalankan tiap 5 detik, menggantikan `delay(5000)` yang bersifat blocking. |
| `if (WiFi.status() == WL_CONNECTED) { Serial.println("Status: Terhubung"); }` | Jika masih terhubung, cukup cetak status. |
| `else { ...; WiFi.disconnect(); WiFi.begin(sta_ssid, sta_password); }` | Jika terputus, `WiFi.disconnect()` membersihkan koneksi lama, lalu `WiFi.begin()` dipanggil ulang — inti dari fitur auto-reconnect. |

**Mengapa `millis()`, bukan `delay()`?** `delay()` menghentikan seluruh program selama waktu tersebut. Dengan `millis()`, program tetap berjalan (non-blocking) sambil berkala memeriksa apakah sudah waktunya cek/reconnect WiFi.

---

## 2. Percobaan 2B — Mode AP+STA

### Kode program lengkap

```cpp
#include <ESP8266WiFi.h>   // sesuai board kamu: ESP8266

// --- Kredensial untuk mode Station (menyambung ke WiFi rumah/router) ---
const char* sta_ssid     = "NAMA_WIFI_ANDA";     // ganti dengan SSID WiFi rumah
const char* sta_password = "PASSWORD_WIFI_ANDA"; // ganti dengan password WiFi rumah

// --- Kredensial untuk mode Access Point (sama seperti kode Percobaan 2B asli) ---
const char* ap_ssid     = "UdinPetot";
const char* ap_password = "Admin1234"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);

  // 1. Set mode gabungan: Access Point + Station berjalan bersamaan
  WiFi.mode(WIFI_AP_STA);

  // 2. Aktifkan Access Point terlebih dahulu (persis seperti kode Percobaan 2B)
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

### Penjelasan baris/bagian kode

| Kode | Penjelasan |
|---|---|
| `const char* sta_ssid` / `sta_password` | Kredensial WiFi rumah/router untuk peran Station — **baru**, tidak ada di kode Percobaan 2B asli. |
| `const char* ap_ssid = "UdinPetot";` / `ap_password` | Kredensial Access Point — **tetap sama** seperti kode asli, tidak diubah. |
| `WiFi.mode(WIFI_AP_STA);` | **Perubahan utama** — kode asli memakai `WiFi.mode(WIFI_AP)` (hanya AP). Diganti `WIFI_AP_STA` agar kedua peran aktif bersamaan. |
| `WiFi.softAP(ap_ssid, ap_password);` + cetak SSID/IP AP | **Tidak berubah** dari kode asli. |
| `WiFi.begin(sta_ssid, sta_password);` | **Baris baru** — memulai koneksi Station ke WiFi rumah, berjalan berdampingan dengan AP yang sudah aktif. |
| `unsigned long startAttempt = millis();` + `while (... && millis() - startAttempt < 10000)` | **Baru** — menunggu koneksi Station selesai, dengan batas waktu 10 detik agar tidak macet jika WiFi rumah tidak tersedia. AP tetap berjalan meski Station gagal. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` (di `setup()`) | **Baru** — melaporkan hasil percobaan koneksi Station: berhasil (tampilkan IP) atau gagal (AP tetap aktif). |
| `int jumlahClient = WiFi.softAPgetStationNum();` (di `loop()`) | **Tidak berubah** dari kode asli — tetap menghitung jumlah perangkat yang terhubung ke AP. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { WiFi.begin(sta_ssid, sta_password); }` (di `loop()`) | **Baru** — memantau status Station; jika terputus, coba sambung ulang tanpa mematikan AP. |

**Ringkasan perubahan dari kode asli Percobaan 2B:**
- `WiFi.mode(WIFI_AP)` → `WiFi.mode(WIFI_AP_STA)`
- Ditambahkan kredensial dan proses koneksi Station (`sta_ssid`, `sta_password`, `WiFi.begin()`, pengecekan status dengan timeout)
- Di `loop()`, ditambahkan pemantauan status Station berdampingan dengan pemantauan jumlah client AP yang sudah ada sebelumnya

**Mengapa mode ini berguna untuk provisioning?** Saat pertama kali dinyalakan (atau kredensial WiFi belum tersedia), ESP8266 tetap menyediakan AP sendiri (`UdinPetot`) agar pengguna bisa menyambung langsung dari smartphone dan membuka halaman konfigurasi. Bersamaan dengan itu, ESP8266 mencoba/menjaga koneksi ke WiFi utama untuk mengirim data — tanpa harus memilih salah satu peran saja.

---

> **Catatan untuk kedua file:** ganti nilai `sta_ssid` dan `sta_password` sesuai jaringan WiFi rumah/hotspot yang kamu gunakan saat praktikum, sebelum di-upload ke board.
