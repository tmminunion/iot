#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <time.h>


// Koneksi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Web server di port 80
WebServer server(80);

// Informasi perangkat
const char* deviceName = "ESP32 Smart Light";
const char* deviceId = "ESP32-001";
String localIP;
String currentDateTime;

// Definisi pin lampu
const int lampu1 = 5;
const int lampu2 = 18;
const int lampu3 = 19;
const int lampu4 = 16;

// Definisi pin switch (NC/NO)
const int switch1 = 32;
const int switch2 = 33;
const int switch3 = 25;
const int switch4 = 26;

// Status lampu
bool statusLampu1 = false;
bool statusLampu2 = false;
bool statusLampu3 = false;
bool statusLampu4 = false;

// Variabel untuk menyimpan status switch sebelumnya
bool lastSwitch1, lastSwitch2, lastSwitch3, lastSwitch4;



// Halaman HTML
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Kontrol Lampu ESP32</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css" rel="stylesheet" />
  <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
</head>
<body class="bg-light">
  <div class="container mt-5">
    <h1 class="text-center">Kontrol Lampu ESP32</h1>
    <div class="row justify-content-center mt-4">
      <div class="col-3">
        <div class="card text-center">
          <div class="card-body">
            <h5 class="card-title">Lampu 1</h5>
            <img id="lampu1" src="https://www.w3schools.com/js/pic_bulboff.gif" width="100" height="100" />
            <button class="btn btn-danger mt-3" onclick="toggleLampu(1)">OFF</button>
          </div>
        </div>
      </div>
      <div class="col-3">
        <div class="card text-center">
          <div class="card-body">
            <h5 class="card-title">Lampu 2</h5>
            <img id="lampu2" src="https://www.w3schools.com/js/pic_bulboff.gif" width="100" height="100" />
            <button class="btn btn-danger mt-3" onclick="toggleLampu(2)">OFF</button>
          </div>
        </div>
      </div>
      <div class="col-3">
        <div class="card text-center">
          <div class="card-body">
            <h5 class="card-title">Lampu 3</h5>
            <img id="lampu3" src="https://www.w3schools.com/js/pic_bulboff.gif" width="100" height="100" />
            <button class="btn btn-danger mt-3" onclick="toggleLampu(3)">OFF</button>
          </div>
        </div>
      </div>
      <div class="col-3">
        <div class="card text-center">
          <div class="card-body">
            <h5 class="card-title">Lampu 4</h5>
            <img id="lampu4" src="https://www.w3schools.com/js/pic_bulboff.gif" width="100" height="100" />
            <button class="btn btn-danger mt-3" onclick="toggleLampu(4)">OFF</button>
          </div>
        </div>
      </div>
    </div>
  </div>

  <script>
    function toggleLampu(lamp) {
      fetch(`/toggle?lamp=${lamp}`)
        .then(response => response.text())
        .then(status => {
          updateLampu(lamp, status);
        });
    }

    function updateLampu(lamp, status) {
      let lampuImg = document.getElementById(`lampu${lamp}`);
      let btn = document.querySelector(`button[onclick='toggleLampu(${lamp})']`);
      if (status === "ON") {
        lampuImg.src = "https://www.w3schools.com/js/pic_bulbon.gif";
        btn.innerText = "ON";
        btn.classList.remove("btn-danger");
        btn.classList.add("btn-success");
      } else {
        lampuImg.src = "https://www.w3schools.com/js/pic_bulboff.gif";
        btn.innerText = "OFF";
        btn.classList.remove("btn-success");
        btn.classList.add("btn-danger");
      }
    }

    // Fungsi untuk memeriksa status lampu secara otomatis setiap detik
    setInterval(() => {
      fetch('/status')
        .then(response => response.json())
        .then(status => {
          for (let i = 1; i <= 4; i++) {
            updateLampu(i, status[`lampu${i}`]);
          }
        });
    }, 1000);  // Update setiap detik
  </script>
</body>
</html>
)rawliteral";



// Fungsi untuk mendapatkan waktu saat ini
String getCurrentDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Waktu tidak tersedia";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

// Fungsi untuk mengirim data perangkat ke server eksternal
void sendDeviceInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.bungtemin.net/nufathome");
    http.addHeader("Content-Type", "application/json");
    
    String jsonPayload = "{";
    jsonPayload += "\"device_name\": \"" + String(deviceName) + "\",";
    jsonPayload += "\"device_id\": \"" + String(deviceId) + "\",";
    jsonPayload += "\"local_ip\": \"" + WiFi.localIP().toString() + "\",";
    jsonPayload += "\"start_time\": \"" + currentDateTime + "\"}";
    
    int httpResponseCode = http.POST(jsonPayload);
    http.end();
  }
}




void sendCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Fungsi untuk menangani halaman utama
void handleRoot() {
  sendCORSHeaders();
  server.send(200, "text/html", htmlPage);
}



// Fungsi untuk menangani permintaan informasi perangkat
void handleDeviceInfo() {
  String info = "{";
  info += "\"device_name\": \"" + String(deviceName) + "\",";
  info += "\"device_id\": \"" + String(deviceId) + "\",";
  info += "\"local_ip\": \"" + localIP + "\",";
  info += "\"start_time\": \"" + currentDateTime + "\"}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
   sendCORSHeaders();
  server.send(200, "application/json", info);
}

// Fungsi untuk menangani kontrol lampu dari web
void handleToggle() {
  if (server.hasArg("lamp")) {
    sendCORSHeaders();
    int lamp = server.arg("lamp").toInt();
    bool newState;

    switch (lamp) {
      case 1:
        statusLampu1 = !statusLampu1;
        digitalWrite(lampu1, statusLampu1);
        newState = statusLampu1;
        break;
      case 2:
        statusLampu2 = !statusLampu2;
        digitalWrite(lampu2, statusLampu2);
        newState = statusLampu2;
        break;
      case 3:
        statusLampu3 = !statusLampu3;
        digitalWrite(lampu3, statusLampu3);
        newState = statusLampu3;
        break;
      case 4:
        statusLampu4 = !statusLampu4;
        digitalWrite(lampu4, statusLampu4);
        newState = statusLampu4;
        break;
      default:
      sendCORSHeaders();
        server.send(400, "text/plain", "Lampu tidak valid");
        return;
    }

   
sendCORSHeaders();
    server.send(200, "text/plain", newState ? "ON" : "OFF");
  } else {
    sendCORSHeaders();
    server.send(400, "text/plain", "Argumen tidak valid");
  }
}

// Endpoint untuk mengirim status terbaru lampu
void handleStatus() {
  String status = "{\"lampu1\":\"" + String(statusLampu1 ? "ON" : "OFF") + "\","
                  "\"lampu2\":\"" + String(statusLampu2 ? "ON" : "OFF") + "\","
                  "\"lampu3\":\"" + String(statusLampu3 ? "ON" : "OFF") + "\","
                  "\"lampu4\":\"" + String(statusLampu4 ? "ON" : "OFF") + "\"}";
                  sendCORSHeaders();
  server.send(200, "application/json", status);
}

void setup() {
  Serial.begin(115200);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
  // Sinkronisasi waktu dari server NTP
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  delay(1000);
  currentDateTime = getCurrentDateTime();

  // Kirim data perangkat ke server eksternal
  sendDeviceInfo();
  // Setup pin lampu
  pinMode(lampu1, OUTPUT);
  pinMode(lampu2, OUTPUT);
  pinMode(lampu3, OUTPUT);
  pinMode(lampu4, OUTPUT);

  // Setup pin switch dengan pull-up
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);

 

  // Setup server jika WiFi terhubung
  if (WiFi.status() == WL_CONNECTED) {
    server.on("/", handleRoot);
     server.on("/device_info", handleDeviceInfo);
    server.on("/toggle", handleToggle);
    server.on("/status", handleStatus);
    server.begin();
  }
}

void loop() {
  // Jika WiFi terputus, server tidak perlu dijalankan
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();  // Hanya jalankan server jika WiFi terhubung
  }

  // Periksa status switch untuk kontrol manual
  bool currentSwitch1 = digitalRead(switch1);
  if (currentSwitch1 != lastSwitch1) {
    statusLampu1 = !statusLampu1;
    digitalWrite(lampu1, statusLampu1);
    lastSwitch1 = currentSwitch1;
  }

  bool currentSwitch2 = digitalRead(switch2);
  if (currentSwitch2 != lastSwitch2) {
    statusLampu2 = !statusLampu2;
    digitalWrite(lampu2, statusLampu2);
   
    lastSwitch2 = currentSwitch2;
  }

  bool currentSwitch3 = digitalRead(switch3);
  if (currentSwitch3 != lastSwitch3) {
    statusLampu3 = !statusLampu3;
    digitalWrite(lampu3, statusLampu3);
    lastSwitch3 = currentSwitch3;
  }

  bool currentSwitch4 = digitalRead(switch4);
  if (currentSwitch4 != lastSwitch4) {
    statusLampu4 = !statusLampu4;
    digitalWrite(lampu4, statusLampu4);
    lastSwitch4 = currentSwitch4;
  }
}
