// This code is not protected by any copyright as far as I know
// It is based on misc public sources on the Internet
// Prepared by Mark Polyak in 2020

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_port>\n", argv[0]);
        return 1;
    }

    // Проверка, что аргумент <server_port> является целым числом
    char* endptr;
    int port = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || port < 1 || port > 65535) {
        printf("Error: Invalid server port specified.\n");
        return 2;
    }


    int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while (1) {
        sock = accept(listener, 0, 0);
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        int bytesReceived;
        while (1) {
            do {
                char buffer[1024];

                // принимаем сообщение от клиента
                bytesReceived = recv(sock, buffer, 1024, 0);

                if (bytesReceived == -1) {
                    perror("recv");
                    exit(4);
                }

                if (bytesReceived == 1 && buffer[0] == '0') {
                    // завершаем работу
                    close(sock);
                    return 0;
                }

                buffer[bytesReceived] = '\0';
                printf("%s", buffer);

                // отправляем сообщение в сокет
                if (send(sock, buffer, strlen(buffer), 0) == -1) {
                    perror("send");
                    return 5;
                }


            } while (bytesReceived == 1023);

            printf("\n");
        }

        close(sock);
    }



    

    return 0;
}
