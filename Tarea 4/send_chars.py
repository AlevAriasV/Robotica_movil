import socket

# Wifi
#UDP_IP = "000.000.0.000" # Indicar IP
#UDP_PORT = 0000 # Indicar puerto

UDP_IP = "192.168.4.1" # Indicar IP
UDP_PORT = 8888 # Indicar puerto
sock = socket.socket(socket.AF_INET, # Internet
                socket.SOCK_DGRAM) # UDP
                
def send_message(message):
    mes = bytes(message, 'ascii')
    sock.sendto(mes, (UDP_IP, UDP_PORT))
    
# 7 avanza llanta delatera izquierda
# 5 frena el coche
if __name__ == '__main__':
    print("Escribe el comando a enviar o 'salir' para terminar el programa.")
    while(True):
        instrucción = input()
        if instrucción == 'salir':
            break
        send_message(instrucción)
    
