#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// LOCAL INCLUDES ================================================
#include "mmio.h"
#include "gpio.c"
// ===============================================================

// DEFINES =======================================================
#define PORT 3512
#define LPD_BASE 0x80000000
#define LPD_SIZE 0x20000000
#define BUFFER_SIZE 0X1000
// ===============================================================

// VARIABLES =====================================================
static uint verbose;
static char buffer[BUFFER_SIZE];
// ===============================================================

// WRITE BYTES ===================================================
static int write_bytes(int fd, void *target, int len) {
    unsigned char *t = target;
    int r = write(fd, t, len);

    if (r != len) {
        printf("write_bytes: %d bytes, expected %d\n", r, len);
        perror("write");
        return 1;
    }
    return 0;
}
// ===============================================================

// READ BYTES ====================================================
static int read_bytes(int fd, void *target, int len) { 
    // len as many as you need exact
    unsigned char *t = target;
    // ? timeout
    while (len > 0) {
        int r = read(fd, t, len);

        if (r < 0) {
            printf("read_bytes: %d bytes, expected %d\n", r, len);
            perror("read");
            return 1;
        }

        t += r;
        len -= r;
    }
    return 0;
}
// ===============================================================

// CMD WRITE =====================================================
int cmd_write(int fd, mmio_t *mmio) {
    int len, length;
    int addr, address;

    uint32_t word;

    // RECEIVE LENGTH ============================================
    if (read_bytes(fd, &len, 4)) return 1;
    length = len;
    // ===========================================================

    // RECEIVE ADDRESS ===========================================
    if (read_bytes(fd, &addr, 4)) return 1;
    address = addr;
    // ===========================================================

    // RECEIVE BUFFER ============================================
    int i = 0;
    int offset = (address - LPD_BASE);

    while (len > 0) {
        if (read_bytes(fd, &word, 4)) return 1;

        // WRITE DEVICE MEMORY ===================================
        if (mmio_write32(mmio, offset, word) < 0) {
            fprintf(stderr, "mmio_write: %s\n", mmio_errmsg(mmio));
            exit(1);
        }
        // =======================================================

        i += 4;
        len -= 4;
        offset += 4;
    }

    if (verbose) printf("\n\tWrite data to  address --> 0x%08X, length = 0x%08X, (%5d) bytes\n", address, length, length);

    return 0;
}
// ===============================================================

// CONVERT INT TO BYTES ==========================================
void int_to_bytes(uint32_t value, char* buffer) {
    memcpy(buffer, &value, sizeof(int));
}
// ===============================================================

// CMD READ ======================================================
int cmd_read(int fd, mmio_t *mmio) {
    int len, length;
    int addr, address;

    if (read_bytes(fd, &len, 4)) return 1;
    
    length = len;
    
    if (read_bytes(fd, &addr, 4)) return 1;
    
    address = addr;

    uint32_t word;
    char temp[4];
    int i = 0;
    int offset = (address - LPD_BASE);
    char buff[length];

    while (len > 0) {
        if (mmio_read32(mmio, offset, &word) < 0) {
            fprintf(stderr, "mmio_read(): %s\n", mmio_errmsg(mmio));
            exit(1);
        }

        int_to_bytes(word, temp);
        memcpy(buff + i, temp, 4);

        i += 4;
        len -= 4;
        offset += 4;
    }

    if (write_bytes(fd, buff, length)) return 1;

    if (verbose) printf("\tRead data from address --> 0x%08X, length = 0x%08X, (%5d) bytes\n", address, length, length);

    return 0;
}
// ===============================================================

// HANDLE DATA ===================================================
int handle_data(int fd, mmio_t *mmio) {
    while (1) {
        char cmd;

        if (read_bytes(fd, &cmd, 1)) {
            printf("\t read cmd '%c'\n", cmd);
            return 1;
        }

        switch (cmd) {
            // cmd write
            case 'w': {
                if (cmd_write(fd, mmio))
                    return 1;
                break;
            }
            // cmd read
            case 'r': {
                if (cmd_read(fd, mmio))
                    return 1;
                break;
            }
            // cmd exit
            case 'x': {
                return -1;
                break;
            }
            // wrong cmd
            default: {
                printf("\tInvalid cmd '%c'\n", cmd);
                return 1;
            }          
        }
    }
}
// ===============================================================

// MAIN ==========================================================
int main(int argc, char** argv) {
    char *v_cmd;
    
    if(argv[1]) v_cmd = argv[1];
    else {
        v_cmd = "-p";
        printf("Use args: [-p | --part] or [-f | --full] or [? | --help]\n");
    }

    if (strcmp(v_cmd, "-p") == 0) verbose = 1;
    else if (strcmp(v_cmd, "--part") == 0) verbose = 1;
    else if (strcmp(v_cmd, "-f") == 0) verbose = 2;
    else if (strcmp(v_cmd, "--full") == 0) verbose = 2; 
    else if (strcmp(v_cmd, "?") == 0) {
        printf("-p, --part  Minimal print\n-f, --full  Full print\n?, --help  Help print");
        return -1;
    }
    else if (strcmp(v_cmd, "--help") == 0) {
        printf("-p, --part  Minimal print\n-f, --full  Full print\n?, --help  Help print");
        return -1;
    }

    int i, s;

    mmio_t *mmio;
    mmio = mmio_new();

    // GPIO Init
    gpio_init(mmio, verbose);

    // Open control module
    if (mmio_open(mmio, LPD_BASE, LPD_SIZE) < 0) {
        fprintf(stderr, "mmio_open(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }

    struct sockaddr_in address;

    opterr = 0;

    if (verbose) printf("Waiting for connection...\n");

    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        perror("socket");
        return 1;
    }

    i = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof i);

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    address.sin_family = AF_INET;

    if (bind(s, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(s, 0) < 0) {
        perror("listen");
        return 1;
    }

    fd_set conn;
    int maxfd = 0;

    FD_ZERO(&conn);
    FD_SET(s, &conn);

    maxfd = s;

    while (1) {
        fd_set read = conn, except = conn;
        int fd;

        if (select(maxfd + 1, &read, 0, &except, 0) < 0) {
            perror("select");
            break;
        }

        for (fd = 0; fd <= maxfd; ++fd) {
            if (FD_ISSET(fd, &read)) {
                if (fd == s) {                    
                    int newfd;
                    socklen_t nsize = sizeof(address);
                    newfd = accept(s, (struct sockaddr *)&address, &nsize);                    
                    printf("\nConnection from ip: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    
                    if (verbose) printf("connection accepted with fd = %d\n", newfd);
                    
                    if (newfd < 0) 
                        perror("accept");

                    else {
                        int flag = 1;
                        int optResult = setsockopt(newfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

                        if (optResult < 0) perror("TCP_NODELAY error");

                        if (newfd > maxfd) maxfd = newfd;

                        FD_SET(newfd, &conn);
                    }
                }

                else {
                    int r = handle_data(fd, mmio);
                    close(fd);
                    FD_CLR(fd, &conn);

                    if (r == -1) {
                        printf("\nConnection from ip: %s:%d Closed!", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                        printf("\n\nWaiting for connection...\n");
                    }
                    else
                        printf("Handle data error return = %d\n", r);
                }
            }

            else if (FD_ISSET(fd, &except)) {
                if (verbose == 2) printf("connection aborted - fd %d\n", fd);

                close(fd);
                FD_CLR(fd, &conn);

                if (fd == s) break;
            }
        }
    }

    mmio_close(mmio);
    mmio_free(mmio);

    return 0;
}
// ===============================================================

