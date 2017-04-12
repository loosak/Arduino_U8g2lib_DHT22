#include "DHT.h"      //https://github.com/adafruit/DHT-sensor-library
#include <Streaming.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

#define DHTPIN D4     // what pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

U8X8_PCD8544_84X48_4W_SW_SPI u8x8(D5, D7, D1, D6, D2);
/* tile_width =  11,	 width of 11*8=88 pixel */
/* tile_hight =  6, */

const long interval = 1000;
unsigned long previousMillis = 0;

void upTime(int sec, char* retStr, int bufferSize) {
	int min = sec / 60;
	int hr = min / 60;
	snprintf( retStr , bufferSize, "%02d:%02d:%02d", hr, min % 60, sec % 60 );
	retStr[bufferSize-1] = '\0'; // ensure a valid terminating zero!
}

void fnprintf(char* retStr, int bufferSize, const char *format, float val) {
  //static char str[6]="";
	char str[6]="";

  dtostrf(val, 3, 1, &str[strlen(str)]); //4 is mininum width, 2 is precision; float value is copied onto str
  snprintf( retStr, bufferSize, format, str );
  retStr[bufferSize-1] = '\0'; // ensure a valid terminating zero!
}


void setup() {
  Serial.begin(115200);

  dht.begin();

  u8x8.begin();

	u8x8.setFont(u8x8_font_pcsenior_f);

  for (uint8_t  i = 0; i < 11; i++) {
    u8x8.drawUTF8(i, 1, "-");
  }

  u8x8.drawString(0, 5, "___________");

/*
	for (uint8_t  i = 0; i < 99; i++) {
		u8x8.setContrast(i);
		Serial.printf("%i , ",i );
		delay(3000);
	}
	u8x8.setContrast(0);
*/

}

void loop() {
  unsigned long currentMillis = millis();
  char buffer[16]=""; // caller-defined buffers: 16 bytes allocated here on the stack.
  //strcpy(buffer, "");
  //memset(buffer, 0, 16); // mandatory reinitialize string

  // if enough millis have elapsed
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      upTime(currentMillis/1000, buffer, sizeof(buffer));
    	Serial << "Buffer: " << buffer << "\t";
    	u8x8.drawString(0,0,buffer);

      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);

      Serial << currentMillis << "\t" << "Humidity: " << h << " %\t" << "Temperature: " << t << " *C\t" << "Heat index: " << hic << " *C " << endl;

      fnprintf( buffer, sizeof(buffer), "Tmp:%s°C", t);
      u8x8.drawUTF8(0,2,buffer);

      fnprintf( buffer, sizeof(buffer), "Hmd:%s\%", h );
      u8x8.drawString(0,3,buffer);

      fnprintf( buffer, sizeof(buffer), "Hic:%s°C", hic );
      u8x8.drawUTF8(0,4,buffer);
  }

}
