import threading
import serial
import time

port = 'COM6' 
baudrate = 38400

ser = serial.Serial(port, baudrate)
logfile_path = 'D:\\University\\uniSemersters\\semester7\\MicroContorolers\\Lab\\final_project\\MicroProcessor-HWs\\Final-Project\\output.log'

with open(logfile_path, 'w') as logfile:
    logfile.write("")


def send_data():
    while True:
        data_to_send = input()
        data_to_send += '\n'
        ser.write(data_to_send.encode())
        

def receive_data():
    while True:
        received_data = ser.readline().decode().strip()
        if received_data:
            print(f"{received_data}")
            with open(logfile_path, 'a') as logfile:
                logfile.write(f"{received_data}\n")

send_thread = threading.Thread(target=send_data)
receive_thread = threading.Thread(target=receive_data)

send_thread.start()
receive_thread.start()

send_thread.join()
receive_thread.join()

ser.close()