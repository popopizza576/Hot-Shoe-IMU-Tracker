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
int calibrateButtonPin = 2;
int calibrateButtonState = 0;
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

void loop() {

    //declaring stuff
    msTimeTrack = millis();
    startButtonState = digitalRead(startButtonPin);
    stopButtonState = digitalRead(stopButtonPin);
    calibrateButtonState = digitalRead(calibrateButtonPin);

    

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

    //updates the motion tracking variables for the mpu6050 (i wanna switch to bno085)
    mpu6050Update();
    
    if (calibrateButtonState == true) {
      normalScreen = false;
      delay(500);
      calibrateGyro();
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

void printMotionData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //Serial.print(frameTotal);
  //Serial.print("Acceleration X: ");
  Serial.println(xAcceleration);
  //Serial.print(", Y: ");
  Serial.println(yAcceleration);
  //Serial.print(", Z: ");
  if (a.acceleration.z < -gravityConst) {
    Serial.println(zAcceleration + gravityConst);
  } else {
    Serial.println(zAcceleration - gravityConst);
  }
  //Serial.println(" m/s^2");

  //Serial.print("Rotation X: ");
  Serial.println(xGyro - gyroXOffset);
  //Serial.print(", Y: ");
  Serial.println(yGyro - gyroYOffset);
  //Serial.print(", Z: ");
  Serial.println(zGyro - gyroZOffset);
  //Serial.println(" rad/s");
  //Serial.println();
  //Serial.println();
  //Serial.println();
  //Serial.println();
  
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
