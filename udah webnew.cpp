#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

#define LED_PIN 12
#define NUM_LEDS 300
#define BUTTON_PIN 5

const char* ssid = "Wokwi-GUEST";
const char* password = "";

WebServer server(80);
CRGB leds[NUM_LEDS];
bool isRunning = false;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/start", []() {
        if (!isRunning) {
            isRunning = true;
            runLEDAnimation();
            freeAnimation();
            FastLED.clear();
            FastLED.show();
            isRunning = false;
            server.send(200, "application/json", "{\"status\": \"done\"}");
        } else {
            server.send(200, "application/json", "{\"status\": \"running\"}");
        }
    });

    server.on("/status", []() {
        server.send(200, "application/json", isRunning ? "{\"status\": \"running\"}" : "{\"status\": \"idle\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
}

void runLEDAnimation() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Blue;
        FastLED.show();
        delay(50);
    }
}

void freeAnimation() {
    unsigned long startTime = millis();
    while (millis() - startTime < 60000) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int colorChoice = random(0, 4);
            if (colorChoice == 0) {
                leds[i] = CRGB::Blue;
            } else if (colorChoice == 1) {
                leds[i] = CRGB::LightBlue;
            } else if (colorChoice == 2) {
                leds[i] = CRGB::White;
            } else {
                leds[i] = CRGB::Blue;
            }
        }
        FastLED.show();
        delay(100);
    }
}
