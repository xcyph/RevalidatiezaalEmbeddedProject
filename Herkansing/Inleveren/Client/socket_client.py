import serial
import socket
import time

# Configuratie
server_ip = '10.42.0.1'  # Vervang met het IP-adres van de server Raspberry Pi
server_port = 8080
serial_port = '/dev/ttyUSB0'  # Pas aan volgens je systeem
baudrate = 115200

def send_to_server(data):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((server_ip, server_port))
            s.sendall(data.encode())
            print(f"Verzonden naar server: {data}")
    except Exception as e:
        print(f"Fout bij het verzenden naar server: {e}")

# Open de seriële poort
try:
    ser = serial.Serial(serial_port, baudrate)
except serial.SerialException as e:
    print(f"Fout bij het openen van de seriële poort: {e}")
    exit(1)

# Lees seriële gegevens en stuur naar de server
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith("TEMP:"):
            send_to_server(line)
    time.sleep(1)
