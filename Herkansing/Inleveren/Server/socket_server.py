import socket
import threading

# Server configuratie
server_ip = '10.42.0.1'  # Luister op alle interfaces
server_port = 8080

# Drempelwaarden voor de temperatuur
upper_threshold = 27.0
lower_threshold = 26.0

# Functie om verbindingen af te handelen
def handle_client(client_socket):
    try:
        while True:
            data = client_socket.recv(1024).decode()
            if not data:
                break
            print(f"Ontvangen: {data}")

            if data.startswith("TEMP:"):
                temp = float(data.split(":")[1])
                if temp > upper_threshold:
                    response = "SERVO:180"
                elif temp < lower_threshold:
                    response = "SERVO:0"
                else:
                    response = "SERVO:NEUTRAL"

                # Stuur commando terug naar de client
                client_socket.sendall(response.encode())
    except Exception as e:
        print(f"Fout: {e}")
    finally:
        client_socket.close()

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((server_ip, server_port))
    server_socket.listen(5)
    print(f"Server luistert op {server_ip}:{server_port}")

    while True:
        client_socket, _ = server_socket.accept()
        client_handler = threading.Thread(target=handle_client, args=(client_socket,))
        client_handler.start()

try:
    start_server()
except KeyboardInterrupt:
    pass
finally:
    pass
