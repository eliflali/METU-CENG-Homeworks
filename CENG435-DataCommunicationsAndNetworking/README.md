# Socket Programming Assignment for Networking Course

## Overview

This UDP File Transfer project implements a simple client-server file transfer system using UDP sockets in Python just like TCP. The client requests files from the server, which then sends the files in packets. Both the client and server handle packet ordering, acknowledgments (ACK), and negative acknowledgments (NACK) to ensure reliable transmission over an unreliable UDP protocol.

## Features

* UDP Sockets: Utilizes UDP protocol for sending and receiving packets.
* File Transfer: Transfers .obj files from server to client.
* Checksum Verification: Ensures data integrity using CRC32 checksum.
* ACK/NACK Handling: Implements ACK for received packets and NACK for missing or corrupted packets.
* Multi-threading: Uses threading for simultaneous packet sending and ACK handling.
* Order Management: Handles out-of-order packets and reassembles them in the correct order.

## Usage

### Server
* Run generateobjects.sh under objects folder
* Run the server script:
python udpserver.py

* The server will wait for the client to be ready before sending files.
  
### Client
* Run the client script:
python udpclient.py

* The client will send a 'ready' message to the server and then start receiving files.
* Received files will be saved as received_file_[file_id].obj.
