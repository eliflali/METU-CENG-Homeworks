
import socket
import time

def receive_file(s, file_path):
     with open(file_path, 'wb') as file:
         while True:
             data = s.recv(1024)
             if not data:
                 break
             file.write(data)

def tcp_client():
     #host = '127.0.0.1'  # Server IP
     host = '172.17.0.2'  # Server IP
     port = 65432        # Port to connect to
     # Start timing here
     start_time = time.time()

     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
         s.connect((host, port))
         for i in range(10):
             receive_file(s, f"received_small-{i}.obj")
             receive_file(s, f"received_large-{i}.obj")
             print("file received", f"received_small-{i}.obj")
             print("file received", f"received_large-{i}.obj")
    
     # End timing here
     end_time = time.time()
     # Calculate and print the total time
     total_time = end_time - start_time
     print(f"Total time taken: {total_time} seconds")

tcp_client()