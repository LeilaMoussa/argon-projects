import serial
import time

# Configure the serial port
# Replace 'COM3' with your Argon's port name
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1) 
time.sleep(2) # Wait for the connection to establish

options = ['red', 'blue', 'green', 'white', 'off']
i = 0
mod = len(options)
while True:
    color = options[i % mod] + '\n'
    i += 1
    try:
        ser.write(color.encode())
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("Exiting")
        ser.close()
    finally:
        time.sleep(1)
