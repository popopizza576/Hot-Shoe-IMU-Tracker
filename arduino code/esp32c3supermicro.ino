#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define OLED screen size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Initialize OLED display (I2C address 0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// define variables yo
int hoursTC = 00;
int minutesTC = 00;
int secondsTC = 00;
int framesTC = 00;
String fullTimeCodeString = "placeholder";
int framerate = 24;
int commonFramerates[] = {24, 30, 60, 120};
int frameDelay = 1000 / framerate;
unsigned long msTimeTrack;
int frameTrackInt = frameDelay;
int secTrackInt = 1000;

void setup() {
    Serial.begin(115200);


    // combine TC values

    fullTimeCodeString = hoursTC + ":"; 
    fullTimeCodeString = fullTimeCodeString + minutesTC;
    fullTimeCodeString = fullTimeCodeString + ":";
    fullTimeCodeString = fullTimeCodeString + secondsTC;
    fullTimeCodeString = fullTimeCodeString + ":"; 
    fullTimeCodeString = fullTimeCodeString + framesTC;
    
    // Start OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }

    display.clearDisplay();  // Clear buffer
    display.setTextSize(2);  // Text size
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println(fullTimeCodeString);
    display.display(); // Show text on screen

    //drawing the framerate display
    display.clearDisplay();  // Clear buffer
    display.setTextSize(2);  // Text size
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(80, 10);
    display.println(framerate);
    display.display(); // Show text on screen
}

void loop() {

    //declaring stuff
    msTimeTrack = millis();

    //uses millis as a reference for frames (but it do a lotta rounding cuz integers)
    if (framesTC < framerate) {
      
      if (msTimeTrack >= frameTrackInt) {
      framesTC++;
      frameTrackInt = frameTrackInt + frameDelay;
      }
      
    } else if (framesTC == framerate) {
      framesTC = 0;
    }

    //links seconds to millis, I think this would be more accurate
    if (secondsTC < 60 && msTimeTrack >= secTrackInt) {
      secondsTC++;
      secTrackInt = secTrackInt + 1000;
    } else if (secondsTC == 60) {
      secondsTC = 0;
      minutesTC++;
    }
    
    //my other if statement :p
    if (minutesTC == 60) {
      minutesTC = 0;
      hoursTC++;
    }

    //Just updates the timecode using the update timecode function
    updateTimeCode();
    
    // updating the displa stuff
    display.clearDisplay();  // Clear buffer
    display.setTextSize(1.5);  // Text size
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println(fullTimeCodeString);
    display.setTextSize(2);
    display.setCursor(100, 10);
    display.println(framerate);
    display.display(); // Show text on screen


}

int updateTimeCode() {
    //updates timecode stuff

    //hours updates
    if (hoursTC < 1) {
      fullTimeCodeString = "00:";
    } else if (hoursTC < 10) {
      fullTimeCodeString = "0" + hoursTC;
      fullTimeCodeString = hoursTC + ":";
    } else {
      fullTimeCodeString = hoursTC + ":";
    }

    //minutes updates
    if (minutesTC < 10) {
      fullTimeCodeString = fullTimeCodeString + "0";
      fullTimeCodeString = fullTimeCodeString + minutesTC;
      fullTimeCodeString = fullTimeCodeString + ":";
    } else {
      fullTimeCodeString = fullTimeCodeString + minutesTC;
      fullTimeCodeString = fullTimeCodeString + ":";
    }

    //seconds updates
    if (secondsTC < 10) {
      fullTimeCodeString = fullTimeCodeString + "0";
      fullTimeCodeString = fullTimeCodeString + secondsTC;
      fullTimeCodeString = fullTimeCodeString + ":";
    } else {
      fullTimeCodeString = fullTimeCodeString + secondsTC;
      fullTimeCodeString = fullTimeCodeString + ":";
    }

    //frames updates

    if (framesTC < 10) {
      fullTimeCodeString = fullTimeCodeString + "0";
      fullTimeCodeString = fullTimeCodeString + framesTC;
    } else {
      fullTimeCodeString = fullTimeCodeString + framesTC;
    }

    //useless return lol
    return 1;
}
