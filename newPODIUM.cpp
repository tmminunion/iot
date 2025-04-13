const int switchPins[4] = {13, 12, 14, 27};
const int relayPins[4]  = {26, 25, 33, 32};
const int irSensorPin   = 34;

bool relayStates[4] = {false, false, false, false};

unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 2000; // tampilkan status tiap 2 detik

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);  // Switch aktif LOW
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);       // Matikan semua relay awalnya
  }

  pinMode(irSensorPin, INPUT);
}

void loop() {
  // Baca saklar dan atur relay sesuai posisi
  for (int i = 0; i < 4; i++) {
    bool switchState = digitalRead(switchPins[i]) == LOW; // aktif LOW
    relayStates[i] = switchState;
    digitalWrite(relayPins[i], switchState ? HIGH : LOW);
  }

  // Sensor IR untuk menyalakan Relay 1 jika aktif
  if (digitalRead(irSensorPin) == HIGH) { // asumsikan HIGH = deteksi
    relayStates[0] = true;
    digitalWrite(relayPins[0], HIGH);
  }

  // Tampilkan status ke Serial setiap 2 detik
  if (millis() - lastStatusTime > statusInterval) {
    Serial.println("=== Status Relay ===");
    for (int i = 0; i < 4; i++) {
      Serial.printf("Relay %d: %s\n", i + 1, relayStates[i] ? "ON" : "OFF");
    }
    Serial.println("====================\n");
    lastStatusTime = millis();
  }

  delay(50);
}
