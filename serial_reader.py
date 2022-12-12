from time import sleep
import serial
import csv

ser = serial.Serial("/dev/ttyACM1", 9600)

while True:
    try:
        sleep(1)
        getVal = ser.readline()
        line = getVal.rstrip()
        line_2 = line.decode()
        print(line_2)
        with open("CO2_data.csv", "a") as f:
            writer = csv.writer(f, delimiter=",")
            writer.writerow([line_2])
    except:
        print("Keyboard Interrupt")
        break
