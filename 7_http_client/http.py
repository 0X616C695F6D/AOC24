import socket
import argparse

"""
HTTP client in python without the use of libraries.

References:
    1. https://www.geeksforgeeks.org/socket-programming-python/
    2. https://internalpointers.com/post/making-http-requests-sockets-python
"""

# Arguments
parser = argparse.ArgumentParser(description="Simple HTTP client")
parser.add_argument("address", type=str, help="hostname")
parser.add_argument("port", type=int, help="port number")
args = parser.parse_args()

# Constants
ADDRESS = args.address
TIMEOUT = 5
PORT = args.port

# Connect to the server
print(f"Connecting to host {ADDRESS}")
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(TIMEOUT)
s.connect((ADDRESS, PORT))

# Create and send a GET request
request = f"GET / HTTP/1.1\r\n" \
          f"Host: {ADDRESS}\r\n" \
          f"Connection: Close\r\n\r\n"
print(f"Request:\n{request}")
s.sendall(request.encode('utf-8'))

# Read off response in chunks
response = b""
while True:
    chunk = s.recv(4096)
    if not chunk:
        break
    response += chunk

# Decode
response = response.decode('utf-8')
headers, body = response.split("\r\n\r\n", 1)

print(f"Headers\n{headers}\n\nBody:\n{body}")
s.close()
