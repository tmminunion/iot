#include <DMD32.h>
#include <SPI.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

// Ukuran Panel P10
#define WIDTH 1
#define HEIGHT 1

// Pin ESP32 ke LED P10
#define P_A 12
#define P_B 13
#define P_C 14
#define P_D 27
#define P_LAT 26
#define P_OE 2
#define P_CLK 4

DMD dmd(WIDTH, HEIGHT);
hw_timer_t *timer = NULL;
volatile bool refreshDisplay = false;

void IRAM_ATTR onTimer() {
  refreshDisplay = true;
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi Timer
  timer = timerBegin(0);                                // Timer 0 untuk ESP32
  timerAttachInterrupt(timer, onTimer);                 // Pasang interrupt handler
  timerAlarm(timer, 5000, true, 0);                     // Interval 5000 mikrodetik (5ms), autoreload aktif, reload_count diabaikan
  timerStart(timer);                                    // Aktifkan timer

  dmd.clearScreen(true);
}

void loop() {
  if (refreshDisplay) {               // Jika timer telah mengeset refreshDisplay = true
    dmd.scanDisplayBySPI();
    refreshDisplay = false;
  }

  dmd.clearScreen(true);               // Hapus layar sebelumnya
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  dmd.drawString(0, 0, "HELLO", 5, GRAPHICS_NORMAL);
  
  delay(500);                          // Delay agar pesan tetap stabil
}
