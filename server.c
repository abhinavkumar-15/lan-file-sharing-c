#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define OUTPUT_FILE "received_file.bin"

static int receive_all(int sock, void *buffer, size_t length) {
    size_t received = 0;
    unsigned char *p = buffer;

    while (received < length) {
        ssize_t n = recv(sock, p + received, length - received, 0);
        if (n <= 0) return -1;
        received += (size_t)n;
    }
    return 0;
}

int main(void) {
    int server_fd, client_fd, opt = 1;
    struct sockaddr_in address;
    unsigned char header[8], buffer[BUFFER_SIZE];
    uint64_t file_size = 0, remaining;
    FILE *output;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind"); close(server_fd); return 1;
    }
    if (listen(server_fd, 1) < 0) {
        perror("listen"); close(server_fd); return 1;
    }

    printf("Server listening on port %d...\nWaiting for a client...\n", PORT);
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept"); close(server_fd); return 1;
    }
    printf("Client connected!\n");

    if (receive_all(client_fd, header, sizeof(header)) < 0) {
        fprintf(stderr, "Failed to receive file size.\n");
        close(client_fd); close(server_fd); return 1;
    }
    for (int i = 0; i < 8; i++) file_size = (file_size << 8) | header[i];

    output = fopen(OUTPUT_FILE, "wb");
    if (!output) {
        perror("fopen"); close(client_fd); close(server_fd); return 1;
    }

    remaining = file_size;
    while (remaining > 0) {
        size_t wanted = remaining < BUFFER_SIZE ? (size_t)remaining : BUFFER_SIZE;
        ssize_t n = recv(client_fd, buffer, wanted, 0);
        if (n <= 0) {
            fprintf(stderr, "Transfer interrupted.\n");
            fclose(output); remove(OUTPUT_FILE);
            close(client_fd); close(server_fd); return 1;
        }
        if (fwrite(buffer, 1, (size_t)n, output) != (size_t)n) {
            perror("fwrite");
            fclose(output); remove(OUTPUT_FILE);
            close(client_fd); close(server_fd); return 1;
        }
        remaining -= (uint64_t)n;
    }

    fclose(output);
    printf("File received successfully!\nSaved as: %s\nSize: %llu bytes\n",
           OUTPUT_FILE, (unsigned long long)file_size);
    close(client_fd);
    close(server_fd);
    return 0;
}
