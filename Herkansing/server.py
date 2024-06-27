import socket
import threading

# Server configuratie
server_ip = '192.168.178.226'
server_port = 8080

# Drempelwaarden voor de temperatuur
upper_threshold = 25.0
lower_threshold = 20.0

# Functie om verbindingen af te handelen
def handle_client(client_socket):
    while True:
        try:
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
            break

    client_socket.close()

# Start de server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((server_ip, server_port))
server_socket.listen(5)
print(f"Server luistert op {server_ip}:{server_port}")

while True:
    client_socket, addr = server_socket.accept()
    print(f"Verbinding van {addr}")
    client_handler = threading.Thread(target=handle_client, args=(client_socket,))
    client_handler.start()
