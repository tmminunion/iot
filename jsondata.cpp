#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
const char* ssid = "nunu";
const char* password = "nufat17a";





// Pin untuk LED dan buzzer
const int ledFree = 13;   // LED Merah untuk "Free"
const int ledReady = 12;  // LED Hijau untuk "Ready"
const int ledOnjob = 14;  // LED Kuning untuk "Onjob"
const int buzzer = 27;    // Buzzer

// NTP configuration
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

// Daftar nama wanita
const char* namaWanita[] = {
  "Lena", "Indah", "Olive", "Denay", "Ella", "Naura",
  "Nurul", "Lia", "Hanna", "Kanza", "Julia", "Rani", "Selly"
};
const int total_nama = 13;

// Daftar status
const char* status[] = { "Ready", "Onjob", "Free" };
const int total_status = 3;

// Fungsi untuk mendapatkan status random
const char* getRandomStatus() {
  return status[random(0, total_status)];
}

// Spinner untuk indikator koneksi
void spinner() {
  static int8_t counter = 0;
  const char* glyphs = "\xa1\xa5\xdb";
  lcd.setCursor(15, 1);
  lcd.print(glyphs[counter++]);
  if (counter == strlen(glyphs)) counter = 0;
}


// Fungsi untuk menampilkan teks dengan animasi huruf satu per satu
void displayAnimatedText(int row, const char* text) {
  lcd.setCursor(0, row);
  for (int i = 0; text[i] != '\0'; i++) {
    lcd.print(text[i]);
    delay(100);  // Waktu delay antar huruf
  }
}

// Fungsi untuk mengontrol LED dan buzzer berdasarkan status
void controlLEDandBuzzer(const char* currentStatus) {
  digitalWrite(ledFree, LOW);
  digitalWrite(ledReady, LOW);
  digitalWrite(ledOnjob, LOW);
  digitalWrite(buzzer, LOW);

  if (strcmp(currentStatus, "Free") == 0) {
    digitalWrite(ledFree, HIGH);
  } else if (strcmp(currentStatus, "Ready") == 0) {
    digitalWrite(ledReady, HIGH);
  } else if (strcmp(currentStatus, "Onjob") == 0) {
    digitalWrite(ledOnjob, HIGH);
  }

  // Buzzer berbunyi sebentar
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
}

// Fungsi untuk menggulir nama dan status
void displayScrollingAbsensi() {
  for (int i = 0; i < total_nama; i++) {
    lcd.clear();
    displayAnimatedText(0, namaWanita[i]);
    const char* randomStatus = getRandomStatus();
    displayAnimatedText(1, randomStatus);
    controlLEDandBuzzer(randomStatus);
    delay(2000);  // Tunggu 2 detik sebelum menggulir ke nama berikutnya
  }
}

// Setup
void setup() {
  Serial.begin(115200);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");

  // Inisialisasi pin LED dan buzzer
  pinMode(ledFree, OUTPUT);
  pinMode(ledReady, OUTPUT);
  pinMode(ledOnjob, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    spinner();
  }
  Serial.println("\n[WiFi] Connected!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");

 
}

// Loop
void loop() {
 
   displayScrollingAbsensi();
}
