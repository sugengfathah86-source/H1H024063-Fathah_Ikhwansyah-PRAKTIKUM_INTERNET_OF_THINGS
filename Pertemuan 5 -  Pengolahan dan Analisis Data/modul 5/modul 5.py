import json                       # Mengolah data berformat JSON
import time                       # Mengambil waktu penerimaan data
import paho.mqtt.client as mqtt  # Library untuk komunikasi MQTT
import pandas as pd               # Mengolah data dalam bentuk tabel
import matplotlib.pyplot as plt  # Membuat grafik suhu

# Konfigurasi MQTT yang harus sama dengan Arduino IDE
BROKER = "broker.hivemq.com"                        # Alamat broker MQTT
PORT = 1883                                         # Port komunikasi MQTT
TOPIC = "unsoed/tk245004/kelompok6/data"            # Topic yang dipantau
JUMLAH_SAMPEL = 30                                  # Jumlah data yang akan diterima

data = [] # Menampung data waktu dan suhu yang diterima

# Fungsi yang dijalankan saat Python terhubung ke broker MQTT
def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    if reason_code == 0: # Kode 0 menandakan koneksi berhasil
        client.subscribe(TOPIC) # Berlangganan pada topic suhu
        print("Subscribe topic:", TOPIC)
        print("Menunggu data suhu dari ESP8266...")

# Fungsi yang dijalankan setiap kali pesan MQTT diterima
def on_message(client, userdata, msg):
    try:
        # Mengubah pesan JSON menjadi dictionary Python
        payload = json.loads(msg.payload.decode("utf-8"))
        # Mengambil nilai suhu dari atribut "suhu"
        suhu = float(payload["suhu"])
    # Menangani data yang formatnya tidak sesuai
    except (ValueError, KeyError, TypeError, json.JSONDecodeError):
        print("Data tidak valid, diabaikan.")
        return

    # Mencatat waktu ketika data diterima
    waktu = time.strftime("%Y-%m-%d %H:%M:%S")
    # Menyimpan waktu dan suhu ke dalam list
    data.append({
        "waktu": waktu,
        "suhu": suhu
    })

    # Menampilkan nomor sampel, waktu, dan suhu
    print(f"{len(data)}/{JUMLAH_SAMPEL}] Waktu: {waktu} | Suhu: {suhu:.2f} °C")

    # Menghentikan koneksi setelah jumlah sampel terpenuhi
    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# Tahap 1: Akuisisi data melalui MQTT
client = mqtt.Client(
    callback_api_version=mqtt.CallbackAPIVersion.VERSION2
)
# Menghubungkan fungsi callback dengan klien MQTT
client.on_connect = on_connect
client.on_message = on_message
print("Menghubungkan ke broker MQTT...")

try:
    # Menghubungkan Python ke broker MQTT
    client.connect(BROKER, PORT, keepalive=60)
    # Menjalankan proses penerimaan pesan MQTT
    client.loop_forever()
except KeyboardInterrupt:
    # Menangani penghentian program oleh pengguna
    print("\nPengambilan data dihentikan oleh pengguna.")
except Exception as error:
    # Menampilkan kesalahan koneksi atau proses MQTT
    print("Terjadi kesalahan koneksi MQTT:", error)

# Tahap 2: Menyimpan dan menganalisis data
if len(data) == 0:
    # Menampilkan peringatan jika tidak ada data diterima
    print("Tidak ada data yang diterima.")
    print("Periksa koneksi ESP8266, broker, dan topic MQTT.")
else:
    # Mengubah list data menjadi DataFrame
    df = pd.DataFrame(data)

    # Menghitung rata-rata suhu
    rata2 = df["suhu"].mean()
    # Menghitung standar deviasi suhu
    std = df["suhu"].std()

    # Menghitung Z-score untuk setiap data
    if pd.isna(std) or std == 0:
        df["zscore"] = 0.0
    else:
        df["zscore"] = (df["suhu"] - rata2) / std

    # Menandai data sebagai anomali jika nilai absolut Z-score > 2
    df["anomali"] = df["zscore"].abs() > 2

    # Menyimpan waktu, suhu, Z-score, dan status anomali ke file CSV
    df.to_csv("data_suhu.csv", index=False)

    # Menampilkan ringkasan hasil analisis
    print("\n========= HASIL ANALISIS DATA =========")
    print(f"Jumlah data : {len(df)}")
    print(f"Rata-rata : {rata2:.2f} °C")
    print(f"Standar deviasi : {std:.2f}")
    print(f"Minimum : {df['suhu'].min():.2f} °C")
    print(f"Maksimum : {df['suhu'].max():.2f} °C")
    print(f"Jumlah anomali : {int(df['anomali'].sum())}")
    print("Data tersimpan di data_suhu.csv")

    # Tahap 3: Visualisasi data suhu
    plt.figure(figsize=(10, 4)) # Mengatur ukuran grafik

    # Menggambar garis perubahan suhu berdasarkan urutan data
    plt.plot(
        df.index + 1,
        df["suhu"],
        marker="o",
        label="Suhu"
    )

    # Memilih data yang terdeteksi sebagai anomali
    anom = df[df["anomali"]]

    # Menampilkan anomali dengan titik merah
    plt.scatter(
        anom.index + 1,
        anom["suhu"],
        color="red",
        s=100,
        label="Anomali",
        zorder=3
    )

    # Menampilkan garis rata-rata suhu
    plt.axhline(
        rata2,
        color="gray",
        linestyle="-",
        label="Rata-rata"
    )

    # Memberikan label dan judul grafik
    plt.xlabel("Urutan data")
    plt.ylabel("Suhu (°C)")
    plt.title("Data Suhu dan Deteksi Anomali")
    plt.legend()         # Menampilkan keterangan grafik
    plt.tight_layout()   # Merapikan tata letak grafik

    # Menyimpan grafik ke file PNG
    plt.savefig("grafik_suhu.png")
    print("Grafik tersimpan di grafik_suhu.png")

    # Menampilkan grafik pada layar
    plt.show()