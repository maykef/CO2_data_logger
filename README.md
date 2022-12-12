# CO2_data_logger
Arduino code for the K30 CO2 and the si7021 sensors with the Adafruit dataloguer shield

Use eeprom if you need to shift the register of the K30 (default is 0x68, but with the datalogger there is an I2C address conflict so it needs to be changed to 0x69)

PCF8523 is required to reset the clock of the datalogger shield.

Serial_reader.py is to read the Arduino port and save a csv file remotely without need to access the physical card.
