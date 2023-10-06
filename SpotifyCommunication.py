import serial
import spotipy
from spotipy.oauth2 import SpotifyOAuth
from decouple import config

CLIENT_ID = config('SPOTIPY_CLIENT_ID')
CLIENT_SECRET = config('SPOTIPY_CLIENT_SECRET')
REDIRECT_URI = config('SPOTIPY_REDIRECT_URI')
BAUD_RATE = 9600

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID,
                                               client_secret=CLIENT_SECRET,
                                               redirect_uri=REDIRECT_URI,
                                               scope="user-modify-playback-state user-read-playback-state"))

# establish connection to arduino
ser = serial.Serial("COM3", BAUD_RATE)

while True:
    if ser.inWaiting():
        command = ser.readline().decode("utf-8").strip()

        if command == "PLAY_PAUSE":
            current_playback = sp.current_playback()
            if current_playback['is_playing']:
                sp.pause_playback()
            else:
                sp.start_playback()
        elif command == "NEXT_TRACK":
            sp.next_track()

# implement
# setup spotify api call to get song name
# poke around see if there is anything else cool
# possibly rename file
# work with display
# maybe integrate spotify api for specific functionality
# need song info for display too
# also add ability to add song into liked playlist
# shuffle between list of predetermined playlists?
# use joystick to go seek in song
