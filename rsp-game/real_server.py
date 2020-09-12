import socket
import json

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
port = 8080
sock.bind(("127.0.0.1", port))

while True:
    data, addr = sock.recvfrom(5555)
    json_obj = data.decode('utf8').replace("'", '"')
    print(json_obj)
