import serial
from time import sleep
import csv

COM = 'COM7'
BAUD = 57600

arduino = serial.Serial(COM, BAUD, timeout = .1)

print('Waiting for device')
sleep(1)
print(arduino.name)

# Opening CSV file
with open('test.csv', 'w', newline='') as file:
	writer = csv.writer(file)
	writer.writerow(["Time (ms)", "Thrust (g)"])

	sleep(1)

	while True:
		# Capture serial output as a decoded string
		val = str(arduino.readline().decode().strip('\r\n')) 
		valA = val.split("/")
		print(val, end="\r", flush=True)

		# Writing to CSV
		writer.writerow([valA[0], valA[1]])