import serial
import pyautogui

ser = serial.Serial("COM3", 9600)

while True:
    if ser.inWaiting():
        command = ser.readline().decode("utf-8").strip()

        if command == "PLAY_PAUSE":
            pyautogui.press("playpause")
        elif command == "NEXT_TRACK":
            pyautogui.press("nexttrack")

# implement
# universal play/pause
# maybe integrate spotify api for specific functionality
# need song info for display too
