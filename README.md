# Hot-Shoe-IMU-Tracker
My hot shoe IMU Tracker is an open-source camera accessory aimed at helping with compositing and camera tracking for CGI with green screens in non-studio environments. It could be used with software like Unreal Engine (maybe if i learn how to use it) or Blender or other computer graphics engines
# This is NOT READY FOR USE YET, STILL WAY EARLY IN DEVELOPMENT

# USAGE
So... you may ask yourself how do I use this thing? Well, in its current implementation, here's how you need to set up your project:

Get a breadboard, and wire up an I2C OLED 0.91 inch screen, MPU 6050, and 3 buttons to an ESP32, then flash the firmware to it

After that, you can record some motion data, which will print all of the values to the console log, which you will then need to copy and paste into a .txt file

Once you've done that, you can find the file path and paste it into the blender script where it says to put your file path, run the script, and that's where I'm at

Again, a work in progress.
