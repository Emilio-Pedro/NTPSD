#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main()
{
    int socket_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char *mensagem = "Olá, servidor UDP!";
    sendto(socket_fd, mensagem, strlen(mensagem), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));

    recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[BUFFER_SIZE - 1] = '\0';
    printf("Resposta do servidor: %s\n", buffer);

    close(socket_fd);
    return 0;
}