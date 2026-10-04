# LAN File Sharing System in C

A small Computer Networks mini-project that transfers one file from a client to a server over TCP on Ubuntu/Linux.

## Requirements
- Ubuntu/Linux
- GCC (`sudo apt install build-essential`)
- Two terminals for a local test, or two computers on the same reachable LAN

## Files
- `server.c` — accepts a TCP connection and saves the incoming file as `received_file.bin`
- `client.c` — connects to the server and sends a file
- `REPORT.md` — short project report for submission

## Compile
Open a terminal in this folder and run:

```bash
gcc server.c -o server -Wall -Wextra
gcc client.c -o client -Wall -Wextra
```

## Test on one computer
Terminal 1:
```bash
./server
```

Terminal 2:
```bash
echo "Hello from the LAN file sharing project!" > sample.txt
./client 127.0.0.1 sample.txt
```

The receiver writes `received_file.bin` in the directory where the server was started. Check it with:

```bash
cat received_file.bin
cmp sample.txt received_file.bin
```

`cmp` prints nothing and returns exit status `0` when the files match.

## Test between two computers
1. Connect both computers to the same trusted network.
2. On the receiving computer, run `hostname -I` to find its LAN IPv4 address.
3. Start `./server` on the receiving computer.
4. On the sending computer, run `./client SERVER_IP /path/to/file`, replacing `SERVER_IP` with the receiver's actual IP address.
5. If a firewall blocks the connection, only on a trusted network you control, allow the port with `sudo ufw allow 8080/tcp`. Remove the rule after testing with `sudo ufw delete allow 8080/tcp`.

## How it works
1. The server creates a TCP socket, binds port 8080, listens, and accepts one client.
2. The client connects to the server.
3. The client sends an 8-byte file-size header, followed by file contents.
4. The server reads the header, receives the specified number of bytes, and writes them to disk.

TCP provides an ordered byte stream. The program loops around `send()` and `recv()` because a single call is not guaranteed to process all requested bytes.

## Limitations
- One client and one file per server run.
- The output name is always `received_file.bin`; an existing file with that name is overwritten.
- No encryption or authentication. Use only on your own machine or a network where you have permission.
- The server trusts the file size sent by the client and is intended for controlled educational testing, not public networks.
