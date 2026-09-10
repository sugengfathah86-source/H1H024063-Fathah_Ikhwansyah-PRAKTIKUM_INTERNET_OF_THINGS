#include <ESP8266WiFi.h>   // ganti dari WiFi.h karena board kamu ESP8266

const char* ap_ssid = "UdinPetot";
const char* ap_password = "Admin1234"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);
  // Set mode WiFi menjadi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress apIP = WiFi.softAPIP();
  Serial.println("Access Point aktif!");
  Serial.print("SSID : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address : ");
  Serial.println(apIP);
}

void loop() {
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah perangkat terhubung: ");
  Serial.println(jumlahClient);
  delay(5000);
}