import socket
import threading
import random
import time

# Función para generar una clave y un valor aleatorio
def generate_random_key_value():
    key = f"key{random.randint(1, 1000000)}"
    value = f"value{random.randint(1, 1000000)}"
    return key, value

# Función para enviar un comando al servidor y recibir una respuesta
def send_command(command, sock):
    sock.sendall(command.encode())
    response = sock.recv(1024).decode()
    print(f"Response: {response}")

# Función que realiza operaciones de inserción o lectura de manera aleatoria
def worker():
    while True:
        # Crear un nuevo socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect(('localhost', 8080))
            
            # Decidir aleatoriamente si insertar o leer
            if random.choice([True, False]):
                # Realizar un INSERT
                key, value = generate_random_key_value()
                command = f"INSERT {key} {value} a\n" #INSERT key21 AAAA a
                print(f"Sending insert command: {command.strip()} from {client_socket.getpeername()}")
                send_command(command, client_socket)
            else:
                # Realizar un READ
                key = f"key{random.randint(1, 1000)}"
                command = f"READ {key} a\n"# READ key21 a
                print(f"Sending read command: {command.strip()} from {client_socket.getpeername()}")
                send_command(command, client_socket)
        
        time.sleep(random.uniform(0.1, 2.0))

# Crear y lanzar múltiples hilos para realizar operaciones concurrentes
def main():
    num_threads = 1000  # Número de hilos concurrentes
    threads = []

    for _ in range(num_threads):
        thread = threading.Thread(target=worker)
        thread.daemon = True
        thread.start()
        # threads.append(thread)

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Interrupted by user")

if __name__ == "__main__":
    main()
