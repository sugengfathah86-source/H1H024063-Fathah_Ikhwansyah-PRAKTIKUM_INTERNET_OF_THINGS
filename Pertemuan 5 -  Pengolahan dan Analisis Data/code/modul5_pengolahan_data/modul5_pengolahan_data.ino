#include <math.h> // Memanggil fungsi matematika untuk perhitungan sinus

// Penghitung jumlah pembacaan suhu
unsigned long jumlahBaca = 0;

// Fungsi mock sebagai pengganti sensor DHT22
float bacaSuhuMock() {
  jumlahBaca++; // Menambah jumlah pembacaan setiap fungsi dipanggil

  // Menghasilkan perubahan suhu dasar secara bertahap
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

  // Menambahkan noise untuk menyimulasikan fluktuasi suhu
  float noise = random(-5, 6) * 0.1;

  // Menggabungkan suhu dasar dan noise
  float suhu = suhuDasar + noise;

  // Menambahkan anomali berupa lonjakan suhu setiap 25 pembacaan
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }

  return suhu; // Mengembalikan nilai suhu simulasi
}

// Pengaturan buffer untuk Moving Average
const int N = 5;      // Menentukan jumlah maksimal data yang dirata-ratakan
float buffer[N];      // Menyimpan data suhu sebelumnya
int indeks = 0;       // Menentukan posisi penyimpanan data berikutnya
int jumlah = 0;       // Menghitung jumlah data yang sudah tersimpan

// Inisialisasi nilai suhu minimum dan maksimum
float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Fungsi untuk menghitung Moving Average
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru; // Menyimpan suhu baru ke buffer
  indeks = (indeks + 1) % N;  // Mengeser indeks secara melingkar

  // Menghitung jumlah data yang tersedia hingga maksimal lima
  if (jumlah < N) {
    jumlah++;
  }

  float total = 0; // Menyiapkan variabel total suhu

  // Menjumlahkan data suhu yang tersimpan
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }

  // Mengembalikan rata-rata dari data yang tersedia
  return total / jumlah;
}

void setup() {
  Serial.begin(115200);        // Memulai komunikasi Serial Monitor
  randomSeed(analogRead(A0));  // Mengatur nilai awal bilangan acak

  // Menampilkan pesan awal pada Serial Monitor
  Serial.println("Simulasi Pengolahan Data Suhu ESP8266");
}

void loop() {
  // Mengambil nilai suhu dari fungsi simulasi
  float suhu = bacaSuhuMock();

  // Menghitung rata-rata bergerak dari data suhu
  float suhuFilter = hitungMovingAverage(suhu);

  // Memperbarui suhu minimum jika ditemukan nilai lebih rendah
  if (suhu < suhuMin) suhuMin = suhu;

  // Memperbarui suhu maksimum jika ditemukan nilai lebih tinggi
  if (suhu > suhuMax) suhuMax = suhu;

  // Menampilkan suhu mentah
  Serial.print("Mentah: ");
  Serial.print(suhu, 2);

  // Menampilkan hasil Moving Average
  Serial.print(" | Filter: ");
  Serial.print(suhuFilter, 2);

  // Menampilkan suhu minimum
  Serial.print(" | Min: ");
  Serial.print(suhuMin, 2);

  // Menampilkan suhu maksimum
  Serial.print(" | Max: ");
  Serial.println(suhuMax, 2);

  delay(2000); // Menunggu dua detik sebelum pembacaan berikutnya
}