
Modifikasi ini membuat ESP32 mencoba menghubungkan ulang (reconnect) secara
otomatis ke jaringan WiFi apabila koneksi terputus, tanpa perlu menekan tombol
reset pada board.

## Penjelasan baris/bagian kode yang ditambahkan

| Kode | Penjelasan |
|---|---|
| `unsigned long previousMillis = 0;` | Variabel untuk menyimpan waktu (dalam milidetik) terakhir kali program melakukan pengecekan status WiFi. Digunakan sebagai pengganti `delay()` agar program tidak berhenti total. |
| `const long reconnectInterval = 5000;` | Menentukan seberapa sering (tiap 5000 ms / 5 detik) ESP32 memeriksa status koneksi dan mencoba reconnect bila perlu. |
| `bool wasConnected = false;` | Menyimpan status koneksi terakhir, berguna bila ingin menambahkan logika tambahan (misalnya event saat status berubah). |
| `void connectWiFi() { ... }` | Fungsi terpisah yang membungkus proses koneksi awal (`WiFi.mode`, `WiFi.begin`, dan pengecekan status), sehingga bisa dipanggil ulang kapan saja tanpa menduplikasi kode. |
| `unsigned long startAttempt = millis();` dan `while (... && millis() - startAttempt < 10000)` | Membatasi waktu percobaan koneksi awal maksimal 10 detik. Ini mencegah program macet selamanya (*infinite loop*) jika WiFi tidak tersedia sama sekali. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` | Setelah batas waktu percobaan habis, program memeriksa apakah berhasil terhubung atau tidak, lalu mencetak pesan serta menyalakan/mematikan LED sesuai hasilnya. |
| `unsigned long currentMillis = millis();` (di dalam `loop()`) | Mengambil waktu saat ini setiap kali `loop()` dijalankan, dibandingkan dengan `previousMillis`. |
| `if (currentMillis - previousMillis >= reconnectInterval) { previousMillis = currentMillis; ... }` | Pola *non-blocking timer*: blok kode di dalamnya hanya dijalankan setiap 5 detik, menggantikan `delay(5000)` yang sifatnya blocking (menghentikan seluruh program selama delay berjalan). |
| `if (WiFi.status() == WL_CONNECTED) { ... }` | Jika masih terhubung, program hanya mencetak status dan memastikan LED menyala. |
| `else { ... WiFi.disconnect(); WiFi.begin(ssid, password); }` | Jika status **tidak** `WL_CONNECTED` (terputus), LED dimatikan, lalu `WiFi.disconnect()` dipanggil untuk membersihkan koneksi lama, kemudian `WiFi.begin(ssid, password)` dipanggil ulang untuk memulai proses reconnect. Baris inilah inti dari fitur auto-reconnect. |

## Mengapa menggunakan `millis()`, bukan `delay()`?

Fungsi `delay()` menghentikan seluruh eksekusi program selama waktu yang
ditentukan, sehingga ESP32 tidak bisa melakukan hal lain (misalnya membaca
sensor) selama proses tersebut. Dengan pendekatan `millis()`, program tetap
dapat berjalan (non-blocking) sambil secara berkala memeriksa apakah sudah
waktunya melakukan pengecekan/reconnect WiFi.

## Cara kerja singkat

1. Saat `setup()`, ESP32 mencoba terhubung ke WiFi (maksimal 10 detik percobaan).
2. Di dalam `loop()`, setiap 5 detik ESP32 memeriksa status koneksi.
3. Jika status `WL_CONNECTED`, tidak ada tindakan khusus selain memastikan LED menyala.
4. Jika status terputus, ESP32 otomatis memanggil `WiFi.disconnect()` lalu `WiFi.begin()` kembali — proses ini berulang setiap 5 detik sampai koneksi berhasil kembali.

Modifikasi ini membuat ESP32 berjalan pada mode **AP+STA**, yaitu tetap
terhubung ke jaringan WiFi rumah (sebagai Station) sekaligus menyediakan
Access Point sendiri yang dapat diakses perangkat lain — misalnya untuk
skenario provisioning (konfigurasi awal perangkat IoT).

## Penjelasan baris/bagian kode yang ditambahkan

| Kode | Penjelasan |
|---|---|
| `const char* sta_ssid` / `sta_password` | Kredensial WiFi rumah/router yang akan disambungkan pada peran Station. |
| `const char* ap_ssid` / `ap_password` | Kredensial Access Point yang dibuat sendiri oleh ESP32, dapat diakses langsung oleh perangkat lain. |
| `WiFi.mode(WIFI_AP_STA);` | **Baris kunci** — mengaktifkan kedua peran WiFi (Access Point dan Station) secara bersamaan, berbeda dengan `WIFI_STA` atau `WIFI_AP` yang hanya mengaktifkan satu peran saja. |
| `WiFi.softAP(ap_ssid, ap_password);` | Mengaktifkan ESP32 sebagai Access Point dengan SSID dan password yang ditentukan. |
| `Serial.println(WiFi.softAPIP());` | Menampilkan alamat IP Access Point (default `192.168.4.1`) agar dapat digunakan perangkat lain untuk terhubung. |
| `WiFi.begin(sta_ssid, sta_password);` | Memulai proses koneksi Station ke jaringan WiFi rumah, berjalan berdampingan dengan AP yang sudah aktif. |
| `while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000)` | Menunggu proses koneksi Station selesai, dengan batas waktu 10 detik agar program tidak macet jika WiFi rumah tidak tersedia — AP tetap berjalan meski Station gagal. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }` | Melaporkan hasil percobaan koneksi Station: berhasil (tampilkan IP) atau gagal (AP tetap aktif, hanya Station yang tidak tersambung). |
| `int jumlahClient = WiFi.softAPgetStationNum();` (di `loop()`) | Menghitung jumlah perangkat yang sedang terhubung ke Access Point ESP32, dicetak setiap 5 detik. |
| `if (WiFi.status() == WL_CONNECTED) { ... } else { WiFi.begin(sta_ssid, sta_password); }` | Memantau status Station; jika terputus, mencoba menyambung ulang **tanpa mematikan AP**, sehingga kedua peran tetap berjalan independen. |

## Mengapa mode ini berguna untuk provisioning?

Pada tahap awal pemasangan perangkat IoT, pengguna sering belum tahu
kredensial WiFi mana yang harus dimasukkan ke firmware. Dengan mode AP+STA:

- ESP32 dapat menyediakan AP sendiri (mis. `ESP32_AccessPoint`) agar pengguna
  bisa terhubung langsung dari smartphone dan membuka halaman web konfigurasi.
- Pada saat yang sama, ESP32 dapat mencoba/menjaga koneksi ke jaringan WiFi
  utama untuk mengirim data atau menerima pembaruan konfigurasi — tanpa harus
  memilih salah satu peran saja.
