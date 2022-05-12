import jetson.inference
import jetson.utils
import serial
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
net = jetson.inference.detectNet("ssd-mobilenet-v2", threshold=0.5)
camera = jetson.utils.videoSource("csi://0")      # '/dev/video0' for V4L2
display = jetson.utils.videoOutput("display://0") # 'my_video.mp4' for file
cmd = '{"servo1": 180,"servo2": 90}' # setup servo
ser.write(cmd)
num = 0
print('entering loop')
while display.IsStreaming():
    img = camera.Capture()
    detections = net.Detect(img)
    print('ok')
    for detection in detections:
	print(detection)
        if detection.ClassID == 44 and num == 0: # 1 is a person and 44 is a bottle
            print(detection)
	    cmd = '{"servo1": 120,"servo2": 90}'
            ser.write(cmd)
            time.sleep(5)
            cmd = '{"servo1": 180,"servo2": 90}'
            ser.write(cmd)
	    num += 1
	    break
    if num == 1:
	break
    display.Render(img)
    display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))
	
while display.IsStreaming():
    img = camera.Capture()
    detections = net.Detect(img)
    for detection in detections:
        if detection.ClassID == 1:
	    if detection.Center[0] > 900:
		cmd = '{"motor1": 500}'
	    elif detection.Center[0] < 380:
		cmd = '{"motor1": -500}'
	    ser.write(cmd)
	    time.sleep(1)
	    cmd = '{"motor1": 0}'
	    ser.write(cmd)

    display.Render(img)
    display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))

ser.close()
