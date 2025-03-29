#include <FastLED.h>

#define LED_PIN 12
#define BUTTON_PIN 5  // Sesuaikan dengan GPIO tombol
#define NUM_LEDS 300  // 5 meter, 60 LED/m

CRGB leds[NUM_LEDS];
bool isRunning = false;

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Gunakan pull-up internal
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {  // Tombol ditekan
        if (!isRunning) {
            isRunning = true;
            runLEDAnimation(); // Nyalakan LED satu per satu
            freeAnimation();   // Animasi bebas selama 1 menit
            FastLED.clear();
            FastLED.show();
            isRunning = false;
        }
    }
}

void runLEDAnimation() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;  
        FastLED.show();
        delay(50); // Atur kecepatan running LED
    }
}

void freeAnimation() {
    unsigned long startTime = millis();
    while (millis() - startTime < 60000) { // 1 menit animasi bebas
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV(random(0, 255), 255, 255);
        }
        FastLED.show();
        delay(100);
    }
}
