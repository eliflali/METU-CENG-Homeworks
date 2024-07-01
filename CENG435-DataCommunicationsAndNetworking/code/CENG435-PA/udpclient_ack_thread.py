import socket
import struct
import threading
import queue
import time
import zlib

def compute_checksum(data):
    # Compute a CRC32 checksum for the given data
    return zlib.crc32(data) & 0xffffffff

def verify_checksum(data, received_checksum):
    # Compute checksum for the received data and compare it with the received checksum
    computed_checksum = compute_checksum(data)
    return computed_checksum == received_checksum

def save_files(received_files):
    # Write each file's data to a separate file
    for file_id, received_packets in received_files.items():
        with open(f'received_file_{file_id}.obj', 'wb') as f:
            # Write the received packets in order to the file
            for i in sorted(received_packets.keys()):
                f.write(received_packets[i])
        print(f"File {file_id} reassembled and saved as 'received_file_{file_id}.obj'.")


def udp_client():
    #server_host = '172.17.0.2'  # Server IP address
    server_host = "127.0.0.1"
    server_port = 20001
    #client_host = '172.17.0.3'  # Client IP address
    client_host = "127.0.0.1"
    client_port = 20002
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.bind((client_host, client_port))

    print("Client started. Sending ready message to the server...")

    # Start timing here to meause time of sending files
    start_time = time.time()


    client_socket.sendto(b'ready', (server_host, server_port))


    # Initialize dictionaries for storing received data and tracking sequence numbers
    received_files = {} # Stores received data packets
    expected_seqs = {} # Stores the next expected sequence number for each file
    last_received_seqs = {} # Flag to control the main loop
    is_continuing = True

    try:
        while is_continuing:
            # Receive a packet from the server
            packet, _ = client_socket.recvfrom(1024)
            # Unpack the file ID, sequence number, and checksum from the packet
            file_id, seq, incoming_checksum = struct.unpack('III', packet[:12])
            data = packet[12:]

            # Check for termination message (sequence number 100000)
            if seq == 100000:
                is_continuing = False
                save_files(received_files)
                break
            # Verify the checksum of the received data
            if not verify_checksum(data, incoming_checksum):
                # If checksum fails, send a NACK to the server
                nack_packet = struct.pack('III', file_id, seq, 1)  # 1 indicates NACK
                client_socket.sendto(nack_packet, (server_host, server_port))
                continue

             # Send an ACK for the received packet
            ack_packet = struct.pack('III', file_id, seq, 0)
            client_socket.sendto(ack_packet, (server_host, server_port))

             # Initialize file storage if it's a new file
            if file_id not in received_files:
                received_files[file_id] = {}
                expected_seqs[file_id] = 0

            # Store the received data
            received_files[file_id][seq] = data
            expected_seq = 0
            # If we haven't received any packets for this file yet, set the expected sequence number
            if expected_seqs[file_id] == 0:
                expected_seqs[file_id] = seq
                expected_seq = seq 
            else:
                expected_seq = expected_seqs[file_id]

            # Process in-order packets - saving them
            # Updating the expected sequence number
            # Send ACK for the received packet
            if seq == expected_seq:
                received_files[file_id][seq] = data
                expected_seqs[file_id] += 1  # Update the expected sequence number for the file

                # Check for any buffered out-of-order packets that can now be processed
                while expected_seqs[file_id] in received_files[file_id]:
                    expected_seq = expected_seqs[file_id]
                    print(f"Received packet: {expected_seq} from file {file_id}")
                    expected_seqs[file_id] += 1

                # Send ACK for the correctly received packet
                ack_packet = struct.pack('III', file_id, seq, 0)
                client_socket.sendto(ack_packet, (server_host, server_port))

            #If packet numbers bigger than expected, store them in the dictionary
            #Buffer and send ACK for the upcoming packets and send NACK for missing packets
            elif seq > expected_seq:
                print(f"Out of order packet: {seq}, expected: {expected_seq} from file {file_id}")
                received_files[file_id][seq] = data
                # Send a NACK for the missing packet
                ack_packet = struct.pack('III', file_id, seq, 0)
                client_socket.sendto(ack_packet, (server_host, server_port))
                nack_packet = struct.pack('III', file_id, expected_seq, 1)
                client_socket.sendto(nack_packet, (server_host, server_port))

    finally:
        # Close the client socket -- this means termination message received
        client_socket.close()
        
    # End timing here
    end_time = time.time()
    # Calculate and print the total time
    total_time = end_time - start_time
    print(f"Total time taken: {total_time} seconds")

    print("Client socket closed.")

if __name__ == "__main__":
    udp_client()