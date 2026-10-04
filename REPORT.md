# Mini Project Report: LAN File Sharing System Using C

## 1. Aim
To develop a simple client-server application that transfers a file over a Local Area Network using TCP socket programming in C.

## 2. Software Requirements
- Ubuntu/Linux operating system
- C compiler (GCC)
- POSIX socket API
- Two terminals or two computers on a trusted network

## 3. Concepts Used
- TCP/IP
- Client-server architecture
- IPv4 addresses and port numbers
- Socket creation, binding, listening, accepting and connecting
- Sending and receiving bytes
- File handling in C

## 4. Architecture
**Sender (Client) → TCP connection over IP network, port 8080 → Receiver (Server) → `received_file.bin`**

## 5. Algorithm
### Server
1. Create a TCP socket.
2. Bind it to port 8080 and listen for a connection.
3. Accept the client.
4. Receive the file-size header.
5. Receive the specified number of bytes and write them to a file.
6. Close the connection and report completion.

### Client
1. Read the file path and server IP from command-line arguments.
2. Open the file and determine its size.
3. Create a TCP socket and connect to the server.
4. Send the file-size header followed by the file bytes.
5. Close the file and socket.

## 6. Testing
Run the server in one terminal:
```bash
./server
```

Run the client in another:
```bash
echo "Network file transfer test" > sample.txt
./client 127.0.0.1 sample.txt
```

Verify the result:
```bash
cat received_file.bin
cmp sample.txt received_file.bin
```

Expected result: the receiver reports a successful transfer, the received file contains the test text, and `cmp` returns status `0` when the files match.

## 7. Result
A basic TCP client-server file transfer program is implemented in C. It can be tested locally or between computers on a reachable LAN.

## 8. Limitations
The program handles one client per run, uses a fixed output filename, and does not provide encryption or authentication. It is intended for educational use on trusted networks.
