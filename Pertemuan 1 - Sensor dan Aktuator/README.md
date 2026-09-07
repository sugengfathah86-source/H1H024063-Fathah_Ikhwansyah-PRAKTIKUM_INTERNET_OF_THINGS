
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
- **[Percobaan 1A — Akuisisi Data Sensor DHT22]**
- **[Percobaan 2A — Kendali Aktuator Relay Berdasarkan Data Sensor]**

## Dokumentasi Foto & Video
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/0d45d3be-a6c0-47c8-b6a5-83c5e0572063" />
<img width="2160" height="3840" alt="image" src="https://github.com/user-attachments/assets/2e968557-99a5-4d5e-81d4-5c7ace77b376" />
