
# Modul 1 — Sensor dan Aktuator (Akuisisi Data Sensor & Kendali Aktuator)

**Nama:** Fathah Ikhwansyah
**NIM:** H1H024063
**Program Studi:** Teknik Komputer — Universitas Jenderal Soedirman

Repositori ini berisi dokumentasi dan source code untuk Modul Praktikum IoT I: Sensor dan Aktuator, mencakup Percobaan 1A (Akuisisi Data Sensor DHT22) dan Percobaan 2A (Kendali Aktuator Relay Berdasarkan Data Sensor), beserta tugas tambahan pada masing-masing percobaan.

## Tujuan Praktikum
1. Memahami konsep akuisisi data sensor pada perangkat IoT berbasis ESP32.
2. Memahami konsep dasar kendali aktuator (relay, motor servo, buzzer) menggunakan ESP32.
3. Mengimplementasikan pembacaan data sensor suhu dan kelembaban menggunakan sensor DHT22.
4. Mengimplementasikan kendali aktuator (relay) secara otomatis berdasarkan data sensor yang diperoleh.
5. Mampu menganalisis hubungan antara data sensor yang diakuisisi dengan respons aktuator pada sistem IoT.

## Ringkasan Dasar Teori
Akuisisi data sensor adalah proses pengambilan besaran fisik dari lingkungan (suhu, kelembaban, dsb.) melalui sensor lalu mengubahnya menjadi sinyal yang dapat diproses mikrokontroler. Kendali aktuator adalah proses sebaliknya: mikrokontroler memberi perintah kepada perangkat keluaran (relay, motor servo, buzzer) untuk melakukan aksi fisik berdasarkan hasil pengolahan data sensor tersebut. Sensor dapat dikelompokkan menjadi **analog** (keluaran tegangan kontinu, dibaca via ADC — mis. LDR, MQ series) dan **digital** (keluaran sinyal biner/protokol tertentu, tanpa perlu ADC — mis. DHT22, HC-SR04, PIR). Pada praktikum ini, ESP32 membaca data suhu & kelembaban dari sensor digital DHT22 melalui pustaka `DHT.h`, lalu menggunakan hasilnya untuk mengendalikan aktuator berupa relay (disimulasikan dengan LED).

## Alat dan Bahan
- Board ESP32 DevKit (1 buah)
- Sensor DHT22 (1 buah)
- Modul Relay 1 channel (1 buah)
- LED (1 buah) sebagai simulasi beban aktuator, dan Resistor 220 Ohm (1 buah)
- Breadboard, Kabel Jumper (secukupnya), Kabel USB
- Laptop/PC dengan Arduino IDE (board manager ESP32 + `DHT sensor library` terpasang)

## Library / Dependencies
| Library | Fungsi |
|---|---|
| `DHT sensor library` (Adafruit) | Komunikasi & pembacaan data sensor DHT22 |
| `Adafruit Unified Sensor` | Dependency wajib bagi `DHT sensor library` |
| Board package `esp32` (Espressif) | Dukungan board ESP32 di Arduino IDE |


## Diagram Rangkaian
<img width="484" height="329" alt="Screenshot 2026-09-07 194139" src="https://github.com/user-attachments/assets/9cd7ac73-9f60-44d6-8751-b80b3fce00c0" />

## Percobaan
- **[Percobaan 1A — Akuisisi Data Sensor DHT22]**

```cpp
#define RELAYPIN 26
const float suhuThreshold = 30.0;
```
- `RELAYPIN` — GPIO 26, pin yang mengendalikan modul relay (atau LED indikator).
- `suhuThreshold` — konstanta ambang batas suhu (°C); dideklarasikan `const` karena nilainya tidak berubah selama program berjalan, hanya dijadikan acuan perbandingan.

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}
```
Selain inisialisasi serial dan sensor (sama seperti Percobaan 1A):
- `pinMode(RELAYPIN, OUTPUT)` — menetapkan pin 26 sebagai output digital, karena ESP32 yang mengirim sinyal kendali ke relay/LED.
- `digitalWrite(RELAYPIN, LOW)` — memastikan aktuator dalam kondisi **mati** saat sistem baru menyala, agar tidak ada beban yang aktif tanpa disengaja.

```cpp
float suhu = dht.readTemperature();

if (isnan(suhu)) {
  Serial.println("Gagal membaca data sensor!");
} else {
  ...
  if (suhu > suhuThreshold) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON");
  } else {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF");
  }
}
```
```cpp
#define RELAYPIN 26
const float suhuThreshold = 30.0;
```
- `RELAYPIN` — GPIO 26, pin yang mengendalikan modul relay (atau LED indikator).
- `suhuThreshold` — konstanta ambang batas suhu (°C); dideklarasikan `const` karena nilainya tidak berubah selama program berjalan, hanya dijadikan acuan perbandingan.

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}
```
Selain inisialisasi serial dan sensor (sama seperti Percobaan 1A):
- `pinMode(RELAYPIN, OUTPUT)` — menetapkan pin 26 sebagai output digital, karena ESP32 yang mengirim sinyal kendali ke relay/LED.
- `digitalWrite(RELAYPIN, LOW)` — memastikan aktuator dalam kondisi **mati** saat sistem baru menyala, agar tidak ada beban yang aktif tanpa disengaja.

```cpp
float suhu = dht.readTemperature();

if (isnan(suhu)) {
  Serial.println("Gagal membaca data sensor!");
} else {
  ...
  if (suhu > suhuThreshold) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON");
  } else {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF");
  }
}
```
```cpp
#define RELAYPIN 26
const float suhuThreshold = 30.0;
```
- `RELAYPIN` — GPIO 26, pin yang mengendalikan modul relay (atau LED indikator).
- `suhuThreshold` — konstanta ambang batas suhu (°C); dideklarasikan `const` karena nilainya tidak berubah selama program berjalan, hanya dijadikan acuan perbandingan.

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}
```
Selain inisialisasi serial dan sensor (sama seperti Percobaan 1A):
- `pinMode(RELAYPIN, OUTPUT)` — menetapkan pin 26 sebagai output digital, karena ESP32 yang mengirim sinyal kendali ke relay/LED.
- `digitalWrite(RELAYPIN, LOW)` — memastikan aktuator dalam kondisi **mati** saat sistem baru menyala, agar tidak ada beban yang aktif tanpa disengaja.

```cpp
float suhu = dht.readTemperature();

if (isnan(suhu)) {
  Serial.println("Gagal membaca data sensor!");
} else {
  ...
  if (suhu > suhuThreshold) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON");
  } else {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF");
  }
}
```
- **[Modifikasi program agar data suhu dan kelembaban dirata-ratakan dari 5 kali 
pembacaan sebelum ditampilkan]**
```cpp
const int JUMLAH_SAMPEL = 5;
```
Konstanta baru yang menentukan berapa kali sensor dibaca sebelum satu nilai rata-rata dihasilkan. Dipisahkan sebagai konstanta agar mudah diubah (misalnya jadi 10 kali) tanpa mengedit logika program.

```cpp
const unsigned long JEDA_ANTAR_BACA = 2000;
```
Konstanta jeda (ms) antar pembacaan **tunggal** di dalam satu siklus rata-rata. Nilainya tetap 2000 ms mengikuti karakteristik minimal DHT22, hanya diberi nama agar lebih jelas maksudnya dibanding angka `2000` langsung ditulis di `delay()`.

```cpp
float totalSuhu = 0;
float totalKelembaban = 0;
int sampelValid = 0;
```
Tiga variabel **akumulator** yang direset ke 0 di **setiap awal `loop()`** (setiap kali akan memulai siklus rata-rata baru):
- `totalSuhu` dan `totalKelembaban` menampung jumlah (bukan rata-rata) dari seluruh pembacaan yang berhasil.
- `sampelValid` menghitung ada berapa banyak pembacaan yang benar-benar berhasil dari 5 percobaan (karena bisa saja 1–2 pembacaan gagal/NaN).

```cpp
for (int i = 0; i < JUMLAH_SAMPEL; i++) {
  ...
}
```
Loop `for` yang mengulang proses pembacaan sensor sebanyak `JUMLAH_SAMPEL` (5) kali. Variabel `i` berfungsi sebagai penghitung iterasi, berjalan dari 0 sampai 4.

```cpp
if (isnan(kelembaban) || isnan(suhu)) {
  Serial.print("Pembacaan ke-");
  Serial.print(i + 1);
  Serial.println(" gagal, dilewati.");
} else {
  totalSuhu += suhu;
  totalKelembaban += kelembaban;
  sampelValid++;
}
```
Sama seperti program asli, setiap pembacaan tetap divalidasi dengan `isnan()`. Bedanya:
- Jika **gagal**, program hanya mencetak pembacaan keberapa yang gagal, lalu **melanjutkan ke iterasi berikutnya** (pembacaan tersebut tidak ikut dijumlahkan).
- Jika **berhasil**, nilainya ditambahkan (`+=`) ke akumulator `totalSuhu`/`totalKelembaban`, dan `sampelValid` ditambah 1 (`sampelValid++`) sebagai penanda satu pembacaan valid lagi telah didapat.

```cpp
delay(JEDA_ANTAR_BACA);
```
Tetap diberi jeda antar pembacaan tunggal di dalam loop `for`, karena sensor DHT22 tidak dapat dibaca lebih cepat dari ±2 detik meskipun kita ingin mengumpulkan banyak sampel.

```cpp
if (sampelValid > 0) {
  float rataSuhu = totalSuhu / sampelValid;
  float rataKelembaban = totalKelembaban / sampelValid;
  ...
} else {
  Serial.println("Semua pembacaan pada siklus ini gagal, tidak ada data untuk dirata-ratakan.");
}
```
Setelah loop `for` selesai (5 kali percobaan pembacaan telah dilakukan):
- Jika **ada minimal satu** pembacaan valid (`sampelValid > 0`), rata-rata dihitung dengan membagi total akumulasi dengan jumlah sampel yang **benar-benar valid** (bukan selalu dibagi 5) — ini penting agar rata-rata tidak menjadi lebih rendah dari seharusnya hanya karena ada pembacaan yang gagal.
- Jika **tidak ada satu pun** pembacaan yang berhasil (`sampelValid == 0`), program tidak melakukan pembagian (menghindari *division by zero*) dan hanya mencetak pesan bahwa seluruh siklus gagal.

## Ringkasan Perbedaan dengan Program Asli
| Aspek | Program Asli (1A) | Program Rata-rata (Tugas Tambahan) |
|---|---|---|
| Jumlah pembacaan per tampilan | 1 kali | 5 kali (diakumulasi) |
| Waktu antar tampilan ke Serial Monitor | ±2 detik | ±10 detik (5 × 2 detik) |
| Penanganan gagal baca | Tampilkan pesan error, lanjut ke siklus berikutnya | Pembacaan yang gagal dilewati, sisanya tetap dirata-rata |
| Nilai yang ditampilkan | Nilai sesaat (instan) | Nilai rata-rata dari beberapa pembacaan (lebih stabil) |

- **[Percobaan 2A — Kendali Aktuator Relay Berdasarkan Data Sensor]**
```cpp
#define RELAYPIN 26
const float suhuThreshold = 30.0;
```
- `RELAYPIN` — GPIO 26, pin yang mengendalikan modul relay (atau LED indikator).
- `suhuThreshold` — konstanta ambang batas suhu (°C); dideklarasikan `const` karena nilainya tidak berubah selama program berjalan, hanya dijadikan acuan perbandingan.

```cpp
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}
```
Selain inisialisasi serial dan sensor (sama seperti Percobaan 1A):
- `pinMode(RELAYPIN, OUTPUT)` — menetapkan pin 26 sebagai output digital, karena ESP32 yang mengirim sinyal kendali ke relay/LED.
- `digitalWrite(RELAYPIN, LOW)` — memastikan aktuator dalam kondisi **mati** saat sistem baru menyala, agar tidak ada beban yang aktif tanpa disengaja.

```cpp
float suhu = dht.readTemperature();

if (isnan(suhu)) {
  Serial.println("Gagal membaca data sensor!");
} else {
  ...
  if (suhu > suhuThreshold) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON");
  } else {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF");
  }
}
```
- **[Percobaan 2A — Kendali Aktuator Relay Berdasarkan Data Sensor]**
```cpp
const float BATAS_ATAS = 30.0;
const float BATAS_BAWAH = 28.0;
```
Menggantikan satu konstanta `suhuThreshold` dengan **dua** ambang batas:
- `BATAS_ATAS` — suhu yang harus dilewati agar aktuator **menyala**.
- `BATAS_BAWAH` — suhu yang harus dilewati (dari atas ke bawah) agar aktuator **mati kembali**. Nilainya sengaja dibuat lebih rendah dari `BATAS_ATAS` untuk membentuk jeda/zona mati di antara keduanya.

```cpp
bool statusAktuator = false;
```
Variabel baru bertipe `bool` (benar/salah) yang **disimpan di luar `loop()`**, sehingga nilainya tetap "diingat" antar-siklus (tidak direset setiap `loop()` dijalankan ulang). Variabel inilah yang membedakan histerisis dari kendali ambang tunggal: keputusan berikutnya bergantung pada status sebelumnya, bukan hanya pada suhu saat ini.

```cpp
if (!statusAktuator && suhu > BATAS_ATAS) {
  statusAktuator = true;
} else if (statusAktuator && suhu < BATAS_BAWAH) {
  statusAktuator = false;
}
```
Menggantikan `if (suhu > suhuThreshold) { ... } else { ... }` pada program asli. Logikanya:
- **Baris 1** (`!statusAktuator && suhu > BATAS_ATAS`): aktuator hanya dinyalakan **jika sebelumnya OFF** (`!statusAktuator`, tanda `!` berarti "bukan/negasi") **dan** suhu sekarang sudah melewati `BATAS_ATAS`.
- **Baris 2** (`statusAktuator && suhu < BATAS_BAWAH`): aktuator hanya dimatikan **jika sebelumnya ON** (`statusAktuator` bernilai `true`) **dan** suhu sekarang sudah turun di bawah `BATAS_BAWAH`.
- Jika **tidak ada satu pun** kondisi di atas yang terpenuhi (misalnya suhu berada di antara 28°C–30°C), tidak ada baris `else` lain yang dijalankan — artinya `statusAktuator` **tetap sama seperti sebelumnya**. Inilah inti dari histerisis: status "mengingat" kondisi terakhirnya selama suhu masih berada di zona mati.

```cpp
digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW);
Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
```
Setelah status diperbarui (atau tetap) oleh blok if-else di atas, baris ini yang benar-benar mengirim sinyal ke pin relay dan mencetak status ke Serial Monitor. Ditulis dengan **ternary operator** (`kondisi ? nilaiJikaBenar : nilaiJikaSalah`) agar satu baris saja cukup untuk kedua kasus, menggantikan blok `if/else` terpisah pada program asli yang menulis `digitalWrite` dan `Serial.println` dua kali.


## Ringkasan Perbedaan dengan Program Asli
| Aspek | Program Asli (2A) | Program Histerisis (Tugas Tambahan) |
|---|---|---|
| Jumlah ambang batas | 1 (`suhuThreshold = 30.0`) | 2 (`BATAS_ATAS = 30.0`, `BATAS_BAWAH = 28.0`) |
| Status aktuator diingat antar-loop? | Tidak (dihitung ulang setiap siklus) | Ya (`statusAktuator` bersifat persisten) |
| Perilaku saat suhu berosilasi di sekitar 30°C | Berpotensi ON/OFF berkedip cepat | Stabil, tidak berkedip selama masih di zona 28°C–30°C |

## Dokumentasi Foto & Video
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/0d45d3be-a6c0-47c8-b6a5-83c5e0572063" />
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/2e968557-99a5-4d5e-81d4-5c7ace77b376" />
