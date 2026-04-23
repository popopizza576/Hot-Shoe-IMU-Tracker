#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

//defines the motion tracker
Adafruit_MPU6050 mpu;

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
int startButtonState = 0;
int startButtonPin = 0;
int stopButtonState = 0;
int stopButtonPin = 1;
int timecodeRunning = 0;
int recordDisplay = 0;
int recBlinkMs = 0;
int frameTotal = 0;
int gravityConst = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Booted");

    //set pin modes
    pinMode(startButtonPin, INPUT);
    pinMode(stopButtonPin, INPUT);

    //define stuff
    gravityConst = 11;

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

    //initialize motion tracker
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);

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
    startButtonState = digitalRead(startButtonPin);
    stopButtonState = digitalRead(stopButtonPin);

    

    //buttons
    if (startButtonState == HIGH) {
      timecodeRunning = true;
    }

    if (stopButtonState == HIGH) {
      timecodeRunning = false;
      framesTC = 0;
      secondsTC = 0;
      minutesTC = 0;
      hoursTC = 0;
      frameTotal = 0;
    }



    if (timecodeRunning == true) {
      updateTimeCodeLogic();
      recordDisplayLogic();
    } else {
      recordDisplay = false;
    }

    if (msTimeTrack >= secTrackInt) {
      secTrackInt = secTrackInt + 1000;
    }

    //Just updates the timecode string using the update timecode function
    updateTimeCodeString();
    
    // updating the displa stuff
    display.clearDisplay();  // Clear buffer
    display.setTextSize(1.5);  // Text size
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 8);
    display.println(fullTimeCodeString);
    display.setTextSize(2);
    display.setCursor(100, 10);
    display.println(framerate);
    if (recordDisplay == true) {
      display.setTextSize(1);
      display.setCursor(10, 22);
      display.println("Recording");
    }
    
    display.display(); // Show text on screen


}

void recordDisplayLogic() {
    if (msTimeTrack - recBlinkMs >= 1000) {
      recBlinkMs = msTimeTrack;
      if (recordDisplay == true) {
        recordDisplay = false;
      } else if (recordDisplay == false) {
        recordDisplay = true;
      }
    } 

}

void updateTimeCodeLogic() {
    //uses millis as a reference for frames (but it do a lotta rounding cuz integers)
    if (framesTC < framerate) {
      
      if (msTimeTrack >= frameTrackInt) {
      framesTC++;
      frameTotal++;
      frameTrackInt = frameTrackInt + frameDelay;
      //prints motion data to console
      printMotionData();
      }
      
    } else if (framesTC == framerate) {
      framesTC = 0;
    }

    //links seconds to millis, I think this would be more accurate
    if (secondsTC < 60 && msTimeTrack >= secTrackInt) {
      secondsTC++;
      //secTrackInt = secTrackInt + 1000;
      //Serial.println(secondsTC);
    } else if (secondsTC == 60) {
      secondsTC = 0;
      minutesTC++;
    }
    
    //my other if statement :p
    if (minutesTC == 60) {
      minutesTC = 0;
      hoursTC++;
    }
    

}

void updateTimeCodeString() {
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


}

void printMotionData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //Serial.print(frameTotal);
  //Serial.print("Acceleration X: ");
  Serial.println(a.acceleration.x);
  //Serial.print(", Y: ");
  Serial.println(a.acceleration.y);
  //Serial.print(", Z: ");
  if (a.acceleration.z < -gravityConst) {
    Serial.println(a.acceleration.z + gravityConst);
  } else {
    Serial.println(a.acceleration.z - gravityConst);
  }
  //Serial.println(" m/s^2");

  //Serial.print("Rotation X: ");
  Serial.println(g.gyro.x);
  //Serial.print(", Y: ");
  Serial.println(g.gyro.y);
  //Serial.print(", Z: ");
  Serial.println(g.gyro.z);
  //Serial.println(" rad/s");
  //Serial.println();
  //Serial.println();
  //Serial.println();
  //Serial.println();
  
}
