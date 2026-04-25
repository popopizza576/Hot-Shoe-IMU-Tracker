import bpy
import math
import sys
import os


dir = os.path.dirname(bpy.data.filepath)
if dir not in sys.path:
    sys.path.append(dir)

#math stuff for rotation
pi = math.pi
rotationMath = pi / 180

#sets the framerate of the clip
framerate = 24
timePerFrame = 1 / framerate

#camera in the scene context
cam = bpy.context.scene.camera

#clears all animation data for the camera
cam.animation_data_clear()

#sets the starting position variables for the camera
xAcceleration = 0
yAcceleration = 0
zAcceleration = 0

xRotation = 90 * rotationMath
yRotation = 0
zRotation = 0

#sets the file path variable for the motion data log (very useful)
filePath = "C:\\Users\\Porter\\Desktop\\hackclub\\blendertest\\april24test.txt"

#the movement data goes in this order:
#1. x acceleration
#2. y acceleration
#3. z acceleration
#4. x gyro
#5. y gyro
#6. z gyro

#sets starting positions
cam.location.x = 0
cam.location.y = 0
cam.location.z = 0

cam.location.x = cam.location.x + xAcceleration
cam.location.y = cam.location.y + yAcceleration
cam.location.z = cam.location.z + zAcceleration

#holy complicated, blender likes to use radians as a measure of rotation instead of normal people degrees, so you have to have the degrees, then multiply that by (pi / 180)

#sets starting rotation
cam.rotation_euler.x = 0
cam.rotation_euler.y = 0
cam.rotation_euler.z = 0

cam.rotation_euler.x = cam.rotation_euler.x + xRotation
cam.rotation_euler.y = cam.rotation_euler.y + (yRotation * timePerFrame)
cam.rotation_euler.z = cam.rotation_euler.z + (zRotation * timePerFrame)

#parses the data log and appends it to a list, as well as making a variable with the amount of frames the animation should have (arguably most important part of the whole script)
with open(filePath, 'r') as file:
    pointList = file.readlines()
    frameAmount = len(pointList) / 6
    print(frameAmount)
   
   
#changes the animation length to match the amount of data points in the motion log 
bpy.context.scene.frame_start = 1
bpy.context.scene.frame_end = int(frameAmount)


#runs through and adds keyframes to the animation with the location from the log parse
for i in range(0, int(frameAmount)):
    currentLine = i * 6
    xAcceleration = float(pointList[currentLine - 1])
    yAcceleration = float(pointList[(currentLine - 1) + 1])
    zAcceleration = float(pointList[(currentLine - 1) + 2])
    xRotation = float(pointList[(currentLine - 1) + 3])
    yRotation = float(pointList[(currentLine - 1) + 4])
    zRotation = float(pointList[(currentLine - 1) + 5])
    cam.location.x = cam.location.x + xAcceleration
    cam.location.y = cam.location.y + yAcceleration
    cam.location.z = cam.location.z + zAcceleration
    cam.rotation_euler.x = cam.rotation_euler.x + (xRotation * timePerFrame)
    cam.rotation_euler.y = cam.rotation_euler.y + (yRotation * timePerFrame)
    cam.rotation_euler.z = cam.rotation_euler.z + (zRotation * timePerFrame)
    cam.location.x = cam.location.x / 10
    cam.location.y = cam.location.y / 10
    cam.location.z = cam.location.z / 10
    print(currentLine)
    cam.keyframe_insert("location", frame=i)
    cam.keyframe_insert("rotation_euler", frame=i)
    
    
    

    
    
