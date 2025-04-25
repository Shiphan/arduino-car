app: arduino.ino
	arduino-cli compile
run: app
	arduino-cli upload
board:
	arduino-cli board list
usbmod:
	sudo chmod o+rw /dev/ttyACM0
