
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

## Skematik Rangkaian Keseluruhan
![Skematik Rangkaian](assets/skematik_rangkaian.png)

## Percobaan
- **[Percobaan 1A — Akuisisi Data Sensor DHT22](Percobaan_1A_Akuisisi_Sensor/README.md)**
- **[Percobaan 2A — Kendali Aktuator Relay Berdasarkan Data Sensor](Percobaan_2A_Kendali_Aktuator/README.md)**

## Pertanyaan Analisis (1.7) — Bahan Laporan Bagian Hasil dan Analisis

**1) Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!**
> **[ISI SENDIRI]** Uraikan di sini apakah hasil pembacaan sensor pada Percobaan 1A sesuai spesifikasi (data tampil tiap 2 detik, error terdeteksi saat gagal baca), dan apakah aktuator pada Percobaan 2A menyala/mati sesuai ambang batas suhu yang diuji, berdasarkan pengamatan aktualmu.

**2) Bagaimana pengaruh akurasi dan waktu tanggap (response time) sensor terhadap kecepatan reaksi aktuator pada sistem IoT?**
Akurasi sensor menentukan seberapa tepat keputusan kendali yang diambil — sensor dengan akurasi rendah dapat memicu aktuator pada kondisi yang sebenarnya belum/sudah tidak memenuhi ambang batas, menghasilkan respons yang keliru. Waktu tanggap (response time) sensor menentukan seberapa cepat perubahan kondisi lingkungan dapat terdeteksi; semakin lambat waktu tanggap sensor (atau semakin panjang jeda pembacaan seperti `delay(2000)` pada DHT22), semakin lambat pula aktuator bereaksi terhadap perubahan nyata di lapangan. Pada sistem yang membutuhkan respons cepat, kombinasi sensor dengan waktu tanggap lambat dan interval baca yang panjang dapat menyebabkan keterlambatan aksi aktuator dibanding kondisi aktual.

**3) Bagaimana cara kerja sistem dalam mengubah data sensor menjadi keputusan kendali aktuator (proses akuisisi hingga aktuasi)?**
Prosesnya berlangsung sebagai siklus tertutup (*closed loop*): (1) sensor mengukur besaran fisik (suhu) dan mengirimkannya ke mikrokontroler; (2) mikrokontroler membaca dan memvalidasi data tersebut (mis. dengan `isnan()`); (3) nilai yang valid dibandingkan dengan ambang batas (threshold) yang telah ditentukan; (4) hasil perbandingan tersebut diterjemahkan menjadi perintah digital (`HIGH`/`LOW`) yang dikirim ke pin aktuator; (5) aktuator (relay/LED) merespons sinyal tersebut dengan aksi fisik (menyala/mati). Siklus ini berulang setiap interval waktu tertentu selama program berjalan.

**4) Bagaimana kombinasi antara akuisisi data sensor dan kendali aktuator dapat digunakan untuk membangun sistem IoT yang responsif terhadap perubahan kondisi lingkungan, misalnya pada sistem smart farming atau smart home?**
Kombinasi ini memungkinkan sistem mengambil keputusan secara otomatis tanpa campur tangan manual terus-menerus. Pada **smart farming**, misalnya, sensor kelembaban tanah dapat memicu aktuator pompa air untuk menyalakan irigasi otomatis ketika tanah terlalu kering, dan mematikannya kembali saat kelembaban sudah cukup. Pada **smart home**, sensor suhu/cahaya dapat mengendalikan aktuator seperti kipas, AC, atau lampu secara otomatis berdasarkan kondisi ruangan aktual. Prinsip yang sama seperti pada praktikum ini — pembacaan sensor, validasi, perbandingan dengan ambang batas, lalu aktuasi — dapat diperluas dengan menambahkan lebih banyak jenis sensor/aktuator dan logika kendali yang lebih kompleks (termasuk histerisis seperti pada tugas tambahan Percobaan 2A) agar sistem lebih stabil dan sesuai kebutuhan nyata di lapangan.

## Dokumentasi Foto & Video
> **[ISI SENDIRI]** Tambahkan foto proses perangkaian/praktikum ke `assets/foto/` dan GIF/video demonstrasi ke `assets/demo/`, lalu tautkan di sini. Sesuai instruksi modul (bagian 1.8), dokumentasi foto/video juga perlu diunggah ke Google Drive dan tautannya disertakan pada Buku Catatan Praktikum.
