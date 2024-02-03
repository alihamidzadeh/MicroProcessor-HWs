import serial
import keyboard

# Change these values as needed
port = 'ttyUSB'
baudrate = 38400

# Define a dictionary mapping each key to its custom character
custom_chars = {
    'A': b'\x41',
    'B': b'\x42',
    'C': b'\x43',
    'D': b'\x44',
    'E': b'\x45',
    'F': b'\x46',
    'G': b'\x47',

    '1': b'\x31',
    '2': b'\x32',
    '3': b'\x33',
    '4': b'\x34',
    '5': b'\x35',
    '6': b'\x36',
    '7': b'\x37',
}

# Open the serial port
print("start")
ser = serial.Serial(port, baudrate)
# print(ser)

keyPressed = False

# Define a function to send the custom character for the pressed key over the serial port


def send_custom_char(event: keyboard.KeyboardEvent):
    global keyPressed
    if not event.name in custom_chars:
        return

    if event.event_type == 'down':
        key = event.name
        if not keyPressed:
            keyPressed = True
            ser.write(custom_chars[key])
            print(custom_chars[key])

    elif event.event_type == 'up':
        keyPressed = False


# Register the function to be called whenever a key is pressed or released
keyboard.hook(send_custom_char)

# Wait for keyboard input indefinitely
keyboard.wait()

#-----------------------------------------------

# import serial
# import msvcrt

# # Change these values as needed
# port = 'COM5' # ttyUSB in Linux
# baudrate = 38400

# # Open the serial port
# ser = serial.Serial(port, baudrate)

# while 1:
#     input_char = msvcrt.getch()
#     if input_char == b'\x03': # CTRL + C
#         break
#     ser.write(input_char.upper())
