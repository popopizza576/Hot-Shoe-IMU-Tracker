#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO08x.h>
//something for sleep mode i think
#include "driver/rtc_io.h"
#include "esp_sleep.h"
//file system
#include "FS.h"
//sd card
#include "SD.h"
//serial
#include "SPI.h"

#define BNO08X_RESET -1

//defines the motion tracker
Adafruit_MPU6050 mpu;
Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

//defines file (test)
File currentFile;

// Define OLED screen size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SD_MOSI     6
#define SD_MISO     5
#define SD_SCLK     4
#define SD_CS       7
#define WAKEUP_GPIO GPIO_NUM_2

// Initialize OLED display (I2C address 0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// define variables yo
int hoursTC = 00;
int minutesTC = 00;
int secondsTC = 00;
int framesTC = 00;
String fullTimeCodeString = "placeholder";
int framerateSelector = 0;
int commonFramerates[] = {24, 30, 60, 120};
int framerate = commonFramerates[framerateSelector];
int frameDelay = 1000 / framerate;
unsigned long msTimeTrack;
int frameTrackInt = frameDelay;
int secTrackInt = 1000;
int startButtonState = 0;
int startButtonPin = 0;
int stopButtonState = 0;
int stopButtonPin = 1;
int powerButtonPin = 2;
int powerButtonState = 0;
int powerRailPin = 3;
int timecodeRunning = 0;
int recordDisplay = 0;
int recBlinkMs = 0;
int frameTotal = 0;
int gravityConst = 0;
int normalScreen = true;
float gyroXOffset = 0;
float gyroYOffset = 0;
float gyroZOffset = 0;
float xAcceleration = 0;
float yAcceleration = 0;
float zAcceleration = 0;
float xGyro = 0;
float yGyro = 0;
float zGyro = 0;
int foundFile = 0;
int lazyCounter = 0;
int frameCorrectionInteger = 0;
unsigned long msSinceOn = 0;
unsigned long elapsed = 0;
String csvString;

void setup() {
    pinMode(powerRailPin, OUTPUT);
    digitalWrite(powerRailPin, HIGH);
    Serial.begin(115200);
    delay(1000);
    Serial.println("Booted");

      esp_deep_sleep_enable_gpio_wakeup(1 << WAKEUP_GPIO, ESP_GPIO_WAKEUP_GPIO_HIGH);



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

  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("SD Card MOUNT FAIL");
  } 
  







    //initialize motion tracker
    //if (!mpu.begin()) {
    //Serial.println("Failed to find MPU6050 chip");
    //while (1) {
    //  delay(10);
    //}
  ///}
  //Serial.println("MPU6050 Found!");


  if (!bno08x.begin_I2C()) { // Start the sensor using I2C
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x Found!");

  setReports();

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

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


void setReports(void) {
  Serial.println("Setting desired reports");

   if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION, 10000)) {
    Serial.println("Could not enable linear accel");
  }

  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000)) {
    Serial.println("Could not enable gyro");
  }
}



void loop() {

    //declaring stuff
    msTimeTrack = millis();
    elapsed = msTimeTrack - msSinceOn;
    startButtonState = digitalRead(startButtonPin);
    stopButtonState = digitalRead(stopButtonPin);
    powerButtonState = digitalRead(powerButtonPin);

    

    //buttons
    if (startButtonState == HIGH) {
      while (true) {

      startButtonState = digitalRead(startButtonPin);
      stopButtonState = digitalRead(stopButtonPin);


        if (startButtonState == LOW) {
          msSinceOn = msTimeTrack;
          elapsed = msTimeTrack - msSinceOn;
          secTrackInt = elapsed;
          timecodeRunning = true;
          Serial.println(msSinceOn);
          findFileName();
          break;
        }

        if(stopButtonState == HIGH) {
          //framerate change

          delay(100);

          if (framerateSelector >= 3) {
            framerateSelector = 0;
          } else {
            framerateSelector++;
          }
          framerate = commonFramerates[framerateSelector];
          break;

        }
      }


    }

    if (stopButtonState == HIGH) {
      timecodeRunning = false;
      framesTC = 0;
      secondsTC = 0;
      minutesTC = 0;
      hoursTC = 0;
      frameTotal = 0;
      frameTrackInt = frameDelay;
      frameCorrectionInteger = 0;
      currentFile.close();
      foundFile = false;
    }



    if (timecodeRunning == true) {
      updateTimeCodeLogic();
      recordDisplayLogic();
    } else {
      recordDisplay = false;
    }


    //Just updates the timecode string using the update timecode function
    updateTimeCodeString();

    //updates the motion tracking variables for the mpu6050 (i wanna switch to bno085)
    //mpu6050Update();
    bno085Update();
    
    if (powerButtonState == true) {
      digitalWrite(powerRailPin, LOW);
      delay(500);
      esp_deep_sleep_start();


    }

    // updating the display stuff

    if (normalScreen == true) {
      updateScreen();
    }
    


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
      
      if (elapsed >= frameTrackInt) {
      framesTC++;
      frameTotal++;

      if (framesTC < framerate) {
        frameTrackInt = frameTrackInt + frameDelay;
      }

      if (frameCorrectionInteger < framerate - 1) {
        frameCorrectionInteger++;
      } else {
        frameCorrectionInteger = 0;
        frameTrackInt = frameTrackInt + (1000 % framerate);
        Serial.println(frameTrackInt);
        Serial.println(elapsed);
      }

      //prints motion data to console
      printMotionData();
      Serial.println(fullTimeCodeString);
      }
      
    } else if (framesTC == framerate && elapsed >= frameTrackInt && frameCorrectionInteger == 0) {
      framesTC = 0;
    }


    //links seconds to millis, I think this would be more accurate
    if (elapsed - secTrackInt >= 1000) {
      secTrackInt += 1000;
      secondsTC++;
      frameTrackInt = frameTrackInt + frameDelay;
      //Serial.println(secondsTC);
        if (secondsTC == 60) {
          secondsTC = 0;
          minutesTC++;
      }
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

void mpu6050Update() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  xAcceleration = a.acceleration.x;
  yAcceleration = a.acceleration.y;
  zAcceleration = a.acceleration.z;
  xGyro = g.gyro.x;
  yGyro = g.gyro.y;
  zGyro = g.gyro.z;
}

void bno085Update() {

  if (bno08x.wasReset()) {
    Serial.println("sensor reset");
    setReports();
  }

  while (bno08x.getSensorEvent(&sensorValue)) {

    switch (sensorValue.sensorId) {

      case SH2_LINEAR_ACCELERATION:
        xAcceleration = sensorValue.un.linearAcceleration.x;
        yAcceleration = sensorValue.un.linearAcceleration.y;
        zAcceleration = sensorValue.un.linearAcceleration.z;
        break;

      case SH2_GYROSCOPE_CALIBRATED:
        xGyro = sensorValue.un.gyroscope.x;
        yGyro = sensorValue.un.gyroscope.y;
        zGyro = sensorValue.un.gyroscope.z;
        break;

    }
  }

  //csvString = String(xAcceleration) + "," + String(yAcceleration) + "," + String(zAcceleration) + "," + String((xGyro - gyroXOffset)) + "," + String((yGyro - gyroYOffset)) + "," + String((zGyro - gyroZOffset));
  //Serial.println(csvString);

}

void printMotionData() {


  csvString = String(xAcceleration) + "," + String(yAcceleration) + "," + String(zAcceleration) + "," + String((xGyro - gyroXOffset)) + "," + String((yGyro - gyroYOffset)) + "," + String((zGyro - gyroZOffset));
  currentFile.println(csvString);
  Serial.println("logged motion data");
  
}

void updateScreen() {
  // updating the display stuff
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

void calibrateGyro(){
  Serial.println("Calibrating the Gyro: Keep Completely Stationary");
  display.clearDisplay();  // Clear buffer
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Calibrating in: 3");
  display.display();
  delay(1000);
  display.clearDisplay();  // Clear buffer
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Calibrating in: 2");
  display.display();
  delay(1000);
  display.clearDisplay();  // Clear buffer
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Calibrating in: 1");
  display.display();
  delay(1000);
  display.clearDisplay();  // Clear buffer
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Calibrating, please wait...");
  display.display();
  int i;
  float sumX = 0;
  float sumY = 0;
  float sumZ = 0;
  int numPoints=1000;
  for (i=0;i<numPoints;i=i+1){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    sumX = sumX + g.gyro.x;
    sumY = sumY + g.gyro.y;
    sumZ = sumZ + g.gyro.z;
    delay(10);
}
gyroXOffset = sumX/numPoints;
gyroYOffset = sumY/numPoints;
gyroZOffset = sumZ/numPoints;

normalScreen = true;
}

void findFileName () {
  lazyCounter = 0;
  while (true) {
    String filename = "/" + String(lazyCounter) + ".csv";

    if (!SD.exists(filename)) {
      currentFile = SD.open(filename, FILE_WRITE);
      break;           
    }

    lazyCounter++;
  }

  }
