
# Modul 5 – Pengolahan dan Analisis Data IoT

**Nama:** Fathah Ikhwansyah | **NIM:** H1H024063 |

README ini berisi jawaban soal yang meminta modifikasi kode:

- **Percobaan 5A, soal no. 4:** menambahkan perhitungan simpangan baku data pada buffer di ESP8266.
- **Percobaan 5B, soal no. 4:** memakai moving average (jendela 5 data) pada data CSV sebelum z-score, lalu membandingkan jumlah anomali.
- 
---

## Percobaan 5A – Soal 4: Menambah Simpangan Baku

### Konsep

Simpangan baku mengukur seberapa tersebar data pada buffer (5 pembacaan terakhir) terhadap rata-ratanya:

```
std = sqrt( Σ (xi − rata-rata)² / n )
```

Rata-rata sudah tersedia dari `hitungMovingAverage()`, jadi nilainya dipakai ulang tanpa menghitung dua kali. Rumus yang dipakai adalah simpangan baku populasi (dibagi `n`), karena buffer dianggap seluruh data yang diamati, dan pembagian `n` lebih ringan serta tidak bermasalah saat `n` kecil.

Contoh perhitungan: buffer = 28,0; 28,2; 28,4; 28,6; 28,8 → rata-rata 28,4 → selisih kuadrat = 0,16; 0,04; 0; 0,04; 0,16 → total 0,40 → 0,40/5 = 0,08 → std = √0,08 ≈ 0,28.

### Kode yang ditambahkan

**1. Fungsi baru `hitungStdDev()`** (ditaruh sebelum `setup()`):

```cpp
float hitungStdDev(float rata2) {
  if (jumlah < 2) return 0.0;

  float jumlahKuadrat = 0;

  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rata2;
    jumlahKuadrat += selisih * selisih;
  }

  return sqrt(jumlahKuadrat / jumlah);
}
```

| Baris | Penjelasan |
|---|---|
| `float hitungStdDev(float rata2)` | Mendefinisikan fungsi yang menerima rata-rata buffer dan mengembalikan simpangan baku bertipe `float`. |
| `if (jumlah < 2) return 0.0;` | Bila data pada buffer kurang dari 2 (pembacaan pertama), sebaran belum bisa dihitung sehingga dikembalikan 0. |
| `float jumlahKuadrat = 0;` | Variabel penampung jumlah kuadrat selisih, dimulai dari 0. |
| `for (int i = 0; i < jumlah; i++)` | Mengulang sebanyak data yang sudah terisi (bukan selalu N), agar perhitungan benar saat buffer belum penuh. |
| `float selisih = buffer[i] - rata2;` | Menghitung jarak setiap data dari rata-rata. |
| `jumlahKuadrat += selisih * selisih;` | Mengkuadratkan selisih agar bernilai positif lalu menjumlahkannya. |
| `return sqrt(jumlahKuadrat / jumlah);` | Membagi dengan jumlah data (varians), lalu mengakarkan (`sqrt`, dari `math.h`) menjadi simpangan baku. |

**2. Pemanggilan di `loop()`**, setelah moving average dihitung:

```cpp
float suhuStd = hitungStdDev(suhuFilter);
```

| Baris | Penjelasan |
|---|---|
| `float suhuStd = hitungStdDev(suhuFilter);` | Memanggil fungsi dengan `suhuFilter` (rata-rata buffer) sebagai argumen dan menyimpan hasilnya di `suhuStd`. Dipanggil setelah `hitungMovingAverage()` supaya buffer sudah berisi data terbaru. |

**3. Penambahan keluaran Serial Monitor.** Baris terakhir `Serial.println(suhuMax, 2);` diubah menjadi `Serial.print(suhuMax, 2);` (tanpa pindah baris), lalu ditambah:

```cpp
Serial.print(" | Std: ");
Serial.println(suhuStd, 2);
```

| Baris | Penjelasan |
|---|---|
| `Serial.print(" | Std: ");` | Mencetak label simpangan baku di baris yang sama dengan Mentah, Filter, Min, dan Max. |
| `Serial.println(suhuStd, 2);` | Mencetak nilai simpangan baku dengan 2 angka desimal, lalu pindah baris. |

### Format keluaran

```
Mentah: 28.16 | Filter: 28.16 | Min: 28.16 | Max: 28.16 | Std: 0.00
```

Saat terjadi lonjakan suhu (mock function menambah +8 °C setiap pembacaan kelipatan 25), nilai `Std` naik tajam, sehingga dapat dijadikan indikator ketidakstabilan data.

---

## Percobaan 5B – Soal 4: Moving Average sebelum Z-score

### Tujuan

Menghaluskan data hasil CSV dengan moving average jendela 5 data, menghitung z-score dari data yang sudah dihaluskan, lalu membandingkan jumlah anomali dengan z-score pada data asli.

### Kode (`modul5_analisis_ma.py`)

| Baris | Penjelasan |
|---|---|
| `import pandas as pd` | Pustaka untuk mengolah data tabel (DataFrame). |
| `import matplotlib.pyplot as plt` | Pustaka untuk membuat grafik. |
| `df = pd.read_csv("data_suhu.csv")` | Membaca 30 data suhu hasil akuisisi 5B, sehingga tidak perlu menjalankan MQTT lagi. |
| `JENDELA = 5` | Ukuran jendela moving average (5 data), sama seperti Percobaan 5A. |
| `rata_asli = df["suhu"].mean()` | Rata-rata data asli. |
| `std_asli = df["suhu"].std()` | Simpangan baku data asli. |
| `df["zscore_asli"] = (df["suhu"] - rata_asli) / std_asli` | Z-score data asli sebagai pembanding. |
| `df["anomali_asli"] = df["zscore_asli"].abs() > 2` | Data dengan nilai mutlak z-score lebih dari 2 ditandai anomali. |
| `df["suhu_ma"] = df["suhu"].rolling(window=JENDELA, min_periods=1).mean()` | Menghitung rata-rata 5 data terakhir untuk setiap baris. `min_periods=1` membuat 4 data awal tetap dihitung dengan data yang tersedia, sama seperti perilaku `jumlah` pada kode ESP. |
| `rata_ma = df["suhu_ma"].mean()` | Rata-rata dari data yang sudah dihaluskan. |
| `std_ma = df["suhu_ma"].std()` | Simpangan baku dari data yang sudah dihaluskan. |
| `df["zscore_ma"] = (df["suhu_ma"] - rata_ma) / std_ma` | Z-score dihitung dari data hasil moving average. |
| `df["anomali_ma"] = df["zscore_ma"].abs() > 2` | Penandaan anomali dengan batas yang sama (\|z\| > 2). |
| `df.to_csv("data_suhu_ma.csv", index=False)` | Menyimpan seluruh hasil ke file CSV baru. |
| `print(...)` (dua baris) | Menampilkan jumlah anomali sebelum dan sesudah moving average. |
| `plt.plot(... "suhu")` dan `plt.plot(... "suhu_ma")` | Menggambar data mentah dan hasil moving average pada grafik yang sama. |
| `plt.scatter(...)` (dua kali) | Titik merah untuk anomali data asli, titik oranye untuk anomali setelah moving average. |
| `plt.xlabel`, `ylabel`, `title`, `legend` | Memberi label sumbu, judul, dan keterangan grafik. |
| `plt.savefig("grafik_suhu_ma.png")` | Menyimpan grafik perbandingan. |

### Hasil Perbandingan

| Metode | Rata-rata (°C) | Std | Jumlah anomali |
|---|---|---|---|
| Z-score data asli | 29,36 | 1,78 | **1** (data ke-18, 38,03 °C, z = 4,86) |
| Z-score setelah moving average (N = 5) | 29,23 | 1,00 | **0** (z maksimum 1,96 pada data ke-22) |

Contoh nilai di sekitar lonjakan:

| Data ke- | Suhu mentah (°C) | Moving average (°C) | z asli | z moving average |
|---|---|---|---|---|
| 17 | 29,11 | 28,92 | −0,14 | −0,31 |
| 18 | **38,03** | 30,78 | **4,86** | 1,55 |
| 19 | 29,05 | 30,83 | −0,17 | 1,60 |
| 21 | 29,28 | 31,00 | −0,04 | 1,76 |
| 22 | 30,11 | 31,20 | 0,42 | 1,96 |

*(Nomor data di sini mengikuti urutan pada grafik, mulai dari 1.)*

### Analisis

1. **Jumlah anomali turun dari 1 menjadi 0.** Moving average dengan jendela 5 membagi lonjakan 38,03 °C ke lima data berikutnya, sehingga pada data ke-18 nilainya turun dari 38,03 °C menjadi sekitar 30,78 °C dan z-score-nya dari 4,86 menjadi 1,55, di bawah batas 2.
2. **Lonjakan tidak hilang, tetapi tersebar.** Nilai moving average tetap tinggi (sekitar 30,8–31,2 °C) selama 5 data setelah lonjakan, sehingga data yang normal ikut terlihat "naik" dan z-score-nya mendekati 2 (maksimum 1,96, nyaris terdeteksi).
3. **Simpangan baku mengecil** (1,78 menjadi 1,00) karena data sudah dihaluskan, tetapi puncak lonjakan ikut mengecil jauh lebih besar daripada simpangan bakunya, sehingga z-score lonjakan turun.
4. **Kesimpulan:** moving average cocok untuk menghaluskan derau kecil, tetapi **tidak cocok untuk mendeteksi lonjakan tunggal** karena meredam anomali yang justru ingin dideteksi. Urutan yang lebih tepat: deteksi anomali dengan z-score pada data mentah, lalu gunakan moving average hanya untuk data yang akan dipakai kendali atau tampilan.

---

## Cara Menjalankan

```bash
pip install paho-mqtt pandas matplotlib
python modul5_analisis_ma.py     # membutuhkan data_suhu.csv di folder yang sama
```

Program 5A dibuka di Arduino IDE (board ESP8266), upload, lalu buka Serial Monitor dengan baud rate 115200.
