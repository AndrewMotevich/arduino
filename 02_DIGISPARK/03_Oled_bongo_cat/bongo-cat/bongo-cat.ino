#include <DigisparkOLED.h>
#include <Wire.h>
// ============================================================================

#include "bongo-cat_128x64c1.h"

void setup() {
  // put your setup code here, to run once:

  oled.begin();
  oled.fill(0xFF);
  delay(1000);
  oled.clear();
  delay(1000);
}

void loop() {
  oled.bitmap(0, 0, 128, 8, bongo_cat);
  delay(500);
  oled.bitmap(0, 0, 128, 8, bongo_cat2);
  delay(500);
  oled.bitmap(0, 0, 128, 8, bongo_cat3);
  delay(500);
}