#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096

static int send_all(int sock, const void *buffer, size_t length) {
    size_t sent = 0;
    const unsigned char *p = buffer;

    while (sent < length) {
        ssize_t n = send(sock, p + sent, length - sent, 0);
        if (n <= 0) return -1;
        sent += (size_t)n;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    FILE *input;
    unsigned char buffer[BUFFER_SIZE], header[8];
    uint64_t file_size;
    size_t n;
    off_t end;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <file_path>\n", argv[0]);
        return 1;
    }

    input = fopen(argv[2], "rb");
    if (!input) { perror("fopen"); return 1; }
    if (fseeko(input, 0, SEEK_END) != 0 || (end = ftello(input)) < 0) {
        perror("file size"); fclose(input); return 1;
    }
    file_size = (uint64_t)end;
    rewind(input);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); fclose(input); return 1; }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != 1) {
        fprintf(stderr, "Invalid IPv4 address.\n");
        fclose(input); close(sock); return 1;
    }

    printf("Connecting to server...\n");
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect"); fclose(input); close(sock); return 1;
    }

    for (int i = 0; i < 8; i++)
        header[i] = (unsigned char)(file_size >> (56 - 8 * i));

    if (send_all(sock, header, sizeof(header)) < 0) {
        perror("send"); fclose(input); close(sock); return 1;
    }

    while ((n = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        if (send_all(sock, buffer, n) < 0) {
            perror("send"); fclose(input); close(sock); return 1;
        }
    }
    if (ferror(input)) {
        fprintf(stderr, "Error reading input file.\n");
        fclose(input); close(sock); return 1;
    }

    printf("File sent successfully: %s\nSize: %llu bytes\n",
           argv[2], (unsigned long long)file_size);
    fclose(input);
    close(sock);
    return 0;
}
