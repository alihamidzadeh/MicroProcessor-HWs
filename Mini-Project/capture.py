# import serial
# import keyboard

# # Change these values as needed
# port = 'COM6'
# baudrate = 38400

# # Define a dictionary mapping each key to its custom character
# custom_chars = {
#     'A': b'\x41',
#     'B': b'\x42',
#     'C': b'\x43',
#     'D': b'\x44',
#     'E': b'\x45',
#     'F': b'\x46',
#     'G': b'\x47',

#     '1': b'\x31',
#     '2': b'\x32',
#     '3': b'\x33',
#     '4': b'\x34',
#     '5': b'\x35',
#     '6': b'\x36',
#     '7': b'\x37',
# }

# # Open the serial port
# print("start")
# ser = serial.Serial(port, baudrate)
# # print(ser)

# keyPressed = False

# # Define a function to send the custom character for the pressed key over the serial port


# def send_custom_char(event: keyboard.KeyboardEvent):
#     global keyPressed
#     if not event.name in custom_chars:
#         return

#     if event.event_type == 'down':
#         key = event.name
#         if not keyPressed:
#             keyPressed = True
#             ser.write(custom_chars[key])
#             print(custom_chars[key])

#     elif event.event_type == 'up':
#         keyPressed = False


# # Register the function to be called whenever a key is pressed or released
# keyboard.hook(send_custom_char)

# # Wait for keyboard input indefinitely
# keyboard.wait()

#-----------------------------------------------

# import serial
# import keyboard
# import msvcrt

# # Change these values as needed
# port = 'COM6' # ttyUSB in Linux
# baudrate = 38400

# # Open the serial port
# ser = serial.Serial(port, baudrate)

# while 1:
#     input_char = msvcrt.getch()
#     print(input_char)
#     if input_char == b'\x03': # CTRL + C
#         break
#     ser.write(input_char.upper())

#-------------------------------------------------------

import threading
import serial
import time

port = 'COM6' 
baudrate = 38400

ser = serial.Serial(port, baudrate)


# ser = serial.Serial(serial_port, baud_rate, timeout=1)

def send_data():
    while True:
        data_to_send = input("Enter data to send: ")
        ser.write(data_to_send.encode())
        time.sleep(0.1)  # Adjust the delay as needed

def receive_data():
    while True:
        received_data = ser.readline().decode().strip()
        if received_data:
            print(f"Received data: {received_data}")

# Create threads for sending and receiving data
send_thread = threading.Thread(target=send_data)
receive_thread = threading.Thread(target=receive_data)

# Start the threads
send_thread.start()
receive_thread.start()

# Wait for the threads to finish (if needed)
send_thread.join()
receive_thread.join()

# Close the serial port when the threads are done (if needed)
ser.close()