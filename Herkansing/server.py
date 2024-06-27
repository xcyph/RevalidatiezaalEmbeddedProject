import socket
import threading

# Server configuratie
server_ip = '192.168.178.226'
server_port = 9999

# Drempelwaarden voor de temperatuur
upper_threshold = 27.0
lower_threshold = 26.0

# Functie om verbindingen af te handelen
def handle_client(client_socket, client_address):
    try:
        # Ontvang de eerste boodschap van de client om te identificeren
        data = client_socket.recv(1024).decode()
        if not data:
            client_socket.close()
            return
        
        # Identificeer de client
        client_type = "Onbekend"
        if "TEMP" in data:
            client_type = "Temperatuursensor"
        elif "SERVO" in data:
            client_type = "Servo"
        
        print(f"{client_type} verbonden van {client_address}")

        # Voer de gebruikelijke verwerking uit
        while True:
            if not data:
                break
            print(f"Ontvangen van {client_type}: {data}")

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
            
            # Ontvang nieuwe gegevens
            data = client_socket.recv(1024).decode()

    except Exception as e:
        print(f"Fout: {e}")

    finally:
        client_socket.close()

# Start de server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((server_ip, server_port))
server_socket.listen(5)
print(f"Server luistert op {server_ip}:{server_port}")

while True:
    client_socket, client_address = server_socket.accept()
    client_handler = threading.Thread(target=handle_client, args=(client_socket, client_address))
    client_handler.start()
