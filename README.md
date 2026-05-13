# What is Hot IMU?
The Hot IMU is an IMU based camera tracker that attaches to any hot or cold shoe on a camera and captures its movement in 3D space. This helps to create dynamic backgrounds using that movement data for compositing green screens instead of static images, which adds a whole new realm of possibilities to what you can do in video post-production and VFX!
# <a href="https://youtu.be/kCqcRCQPGMA">Watch the video here!</a>
<img width="1920" height="720" alt="github" src="https://github.com/user-attachments/assets/01b7f6c6-6e00-4b36-9539-d618ba0f1045" />

# Why use Hot IMU?
The Hot IMU is a great alternative to current Vive-tracker-based solutions for green screen compositing. The Hot IMU does not require any base stations or setup, just put it on and click go, and it will record your motion data.

The Hot IMU is also incredibly affordable compared to the current Vive-tracker-based solutions. It's also extremely affordable, not requiring base stations, which means you can use it virtually anywhere!

# Zine Page

<img width="1398" height="2000" alt="hot IMU" src="https://github.com/user-attachments/assets/d5f9d178-13bb-4bbe-b212-715c44600895" />


# Assembly
The Hot IMU is a compact ESP-32c3-based device, with a BNO085 IMU and OLED screen. The IMU and OLED screen mount to the PCB with breadboard pins. The battery mounts using a JST 2-pin 1.5mm connector.

### PCB

<img width="1442" height="720" alt="Screenshot 2026-05-06 235348" src="https://github.com/user-attachments/assets/4f673a5f-4457-492b-acab-eb5d2e58b116" />

### Schematic

<img width="1395" height="568" alt="Screenshot 2026-05-08 005708" src="https://github.com/user-attachments/assets/7cb6f70e-37ae-472f-b768-d3b1ab18f5b9" />

### BOM

You can access my BOM <a href="https://github.com/popopizza576/Hot-IMU/blob/main/BOM.csv">here</a>

All parts are sourced through Digikey, total being around $25 not including PCB or shipping. 

**NOTE**

To build this, you must have access to a 3D Printer and self-tapping M2 screws.

**Alongside this, parts not included in the BOM include:**

BNO085 module (available on AliExpress)

0.91-inch i2c OLED Display Module (available on AliExpress)

# Usage
The Hot IMU records movement data to the SD card in CSV format. It records one frame of motion data per frame, and the framerate is specified on the OLED screen.

You can change the framerate by holding down the GO button and pressing the STOP button. The built-in framerates are 24, 30, 60, and 120. the default framerate is 24.

After you've got your CSV file of the motion data recorded, you need to create a Blender scene and find the file path. There is a blender script you need to put in the Scripting tab in Blender.

The Blender Script will parse through the motion data and create an animation of the length of the duration of the data you recorded, then it creates 1 frame of animation for every frame and maps your camera movement to the motion capture.

After you render your Blender animation, just sync up your animation and footage from your camera in your editing software of choice, and key out the background!
<img width="1371" height="676" alt="Screenshot 2026-05-06 235805" src="https://github.com/user-attachments/assets/fe465661-1633-49ed-b4d7-f0ed2e90a645" />
