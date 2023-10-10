import serial
import spotipy
import time
import atexit
from spotipy.oauth2 import SpotifyOAuth
from decouple import config

CLIENT_ID = config('SPOTIPY_CLIENT_ID')
CLIENT_SECRET = config('SPOTIPY_CLIENT_SECRET')
REDIRECT_URI = config('SPOTIPY_REDIRECT_URI')
BAUD_RATE = 9600

# establish connection to arduino
ser = serial.Serial("COM3", BAUD_RATE)
HEARTBEAT_INTERVAL = 1

log_file = open("arduino_log.txt", "a")

def close_serial():
    ser.close()
    log_file.close()
    print("Serial connection closed!")

# close serial port when program finishes
atexit.register(close_serial)

# authenticate spotify api
sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID,
                                               client_secret=CLIENT_SECRET,
                                               redirect_uri=REDIRECT_URI,
                                               scope="user-modify-playback-state user-read-playback-state"))

last_track_name = ""

while True:
    current_playback = sp.current_playback()

    if current_playback and current_playback['item']:
        track_name = current_playback['item']['name']

        # need to combine all artists together into string
        artist_name = ', '.join([artist['name'] for artist in current_playback['item']['artists']])

        combined_track_artist = track_name + "|" + artist_name + "\n"
        if track_name != last_track_name:
            # send heartbeat when new song appears
            # need to do this before new song so display does not immediately clear when heartbeat invalid
            ser.write("HEARTBEAT\n".encode())
            ser.write(combined_track_artist.encode())
            last_track_name = track_name

    if ser.inWaiting():
        command = ser.readline().decode("utf-8").strip()

        log_file.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - Received: {command}\n")
        log_file.flush()

        if command == "PLAY_PAUSE":
            if current_playback and current_playback['is_playing']:
                sp.pause_playback()
            else:
                sp.start_playback()
        elif command == "NEXT_TRACK":
            sp.next_track()

    time.sleep(HEARTBEAT_INTERVAL)

# add title scrolling if too long
# also add ability to add song into liked playlist
# shuffle between list of predetermined playlists?
