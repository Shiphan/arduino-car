app: arduino-car.ino
	arduino-cli compile
run: app
	arduino-cli upload
board:
	arduino-cli board list
usbmod:
	sudo chmod o+rw /dev/serial/by-id/usb-Arduino__www.arduino.cc__Arduino_14101-if00
