#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Informasi jaringan WiFi
const char* ssid = "nunu";       // Ganti dengan nama WiFi Anda
const char* password = ""; // Ganti dengan password WiFi Anda

// Definisi pin untuk LED
const int merah = 13;   // LED Merah
const int hijau = 12;   // LED Hijau
const int kuning = 14;  // LED Kuning

// Status lampu
bool statusMerah = false;
bool statusHijau = false;
bool statusKuning = false;

// Inisialisasi WebServer pada port 80
WebServer server(80);

// Inisialisasi LCD (misalnya 16x2 dengan I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fungsi untuk memperbarui status di layar
void updateLCD(const String& message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status LED:");
  lcd.setCursor(0, 1);
  lcd.print(message);
}

// Fungsi untuk menyalakan LED
void handleLampuMerahOn() {
  digitalWrite(merah, HIGH);
  statusMerah = true;
  updateLCD("Merah: ON");
   server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Merah: ON");
}

void handleLampuMerahOff() {
  digitalWrite(merah, LOW);
  statusMerah = false;
  updateLCD("Merah: OFF");
   server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Merah: OFF");
}

void handleLampuHijauOn() {
  digitalWrite(hijau, HIGH);
  statusHijau = true;
  updateLCD("Hijau: ON");
  server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Hijau: ON");
}

void handleLampuHijauOff() {
  digitalWrite(hijau, LOW);
  statusHijau = false;
  updateLCD("Hijau: OFF");
   server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Hijau: OFF");
}

void handleLampuKuningOn() {
  digitalWrite(kuning, HIGH);
  statusKuning = true;
  updateLCD("Kuning: ON");
   server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Kuning: ON");
}

void handleLampuKuningOff() {
  digitalWrite(kuning, LOW);
  statusKuning = false;
  updateLCD("Kuning: OFF");
   server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "text/html", "Lampu Kuning: OFF");
}

// Fungsi untuk mengambil status semua lampu dalam format JSON
void handleStatusLampu() {
  String json = "{";
  json += "\"merah\": " + String(statusMerah ? "true" : "false") + ",";
  json += "\"hijau\": " + String(statusHijau ? "true" : "false") + ",";
  json += "\"kuning\": " + String(statusKuning ? "true" : "false");
  json += "}";
 server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send(200, "application/json", json);
}

// Fungsi untuk halaman utama
void handleRoot() {
  String html = "<h1>Kontrol Lampu</h1>";
  html += "<p><b>Lampu Merah: </b><span id='statusMerah'></span></p>";
  html += "<p><b>Lampu Hijau: </b><span id='statusHijau'></span></p>";
  html += "<p><b>Lampu Kuning: </b><span id='statusKuning'></span></p>";
  
  // Menampilkan gambar lampu
  html += "<p><img id='imgMerah' src='https://www.w3schools.com/js/pic_bulboff.gif' alt='Lampu Merah' width='50'></p>";
  html += "<p><img id='imgHijau' src='https://www.w3schools.com/js/pic_bulboff.gif' alt='Lampu Hijau' width='50'></p>";
  html += "<p><img id='imgKuning' src='https://www.w3schools.com/js/pic_bulboff.gif' alt='Lampu Kuning' width='50'></p>";
  
  html += "<p><a href='/merah/on'>Lampu Merah ON</a></p>";
  html += "<p><a href='/merah/off'>Lampu Merah OFF</a></p>";
  html += "<p><a href='/hijau/on'>Lampu Hijau ON</a></p>";
  html += "<p><a href='/hijau/off'>Lampu Hijau OFF</a></p>";
  html += "<p><a href='/kuning/on'>Lampu Kuning ON</a></p>";
  html += "<p><a href='/kuning/off'>Lampu Kuning OFF</a></p>";
  
  html += "<script>";
  html += "setInterval(function() {";
  html += "fetch('/status').then(response => response.json()).then(data => {";
  html += "document.getElementById('statusMerah').innerText = data.merah ? 'ON' : 'OFF';";
  html += "document.getElementById('statusHijau').innerText = data.hijau ? 'ON' : 'OFF';";
  html += "document.getElementById('statusKuning').innerText = data.kuning ? 'ON' : 'OFF';";
  html += "document.getElementById('imgMerah').src = data.merah ? 'https://www.w3schools.com/js/pic_bulbon.gif' : 'https://www.w3schools.com/js/pic_bulboff.gif';";
  html += "document.getElementById('imgHijau').src = data.hijau ? 'https://www.w3schools.com/js/pic_bulbon.gif' : 'https://www.w3schools.com/js/pic_bulboff.gif';";
  html += "document.getElementById('imgKuning').src = data.kuning ? 'https://www.w3schools.com/js/pic_bulbon.gif' : 'https://www.w3schools.com/js/pic_bulboff.gif';";
  html += "});";
  html += "}, 1000);"; // Memperbarui setiap 1 detik
  html += "</script>";
  
  server.send(200, "text/html", html);
}

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Inisialisasi LED sebagai OUTPUT
  pinMode(merah, OUTPUT);
  pinMode(hijau, OUTPUT);
  pinMode(kuning, OUTPUT);

  // Inisialisasi LCD
  lcd.init();  // 16 karakter, 2 baris
  lcd.backlight();   // Menyalakan backlight LCD

  // Hubungkan ke WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Rute untuk webserver
  server.on("/", handleRoot);                    // Halaman utama
  server.on("/merah/on", handleLampuMerahOn);    // Lampu merah ON
  server.on("/merah/off", handleLampuMerahOff);  // Lampu merah OFF
  server.on("/hijau/on", handleLampuHijauOn);    // Lampu hijau ON
  server.on("/hijau/off", handleLampuHijauOff);  // Lampu hijau OFF
  server.on("/kuning/on", handleLampuKuningOn);  // Lampu kuning ON
  server.on("/kuning/off", handleLampuKuningOff); // Lampu kuning OFF
  server.on("/status", handleStatusLampu); // Rute untuk mendapatkan status lampu
  
  // Jalankan webserver
  server.begin();
  Serial.println("Webserver started!");
}

void loop() {
  server.handleClient(); // Tangani permintaan klien
}

