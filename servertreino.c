#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main()
{
    int socket_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Servidor UDP iniciado na porta %d\n", PORT);

    recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
    buffer[BUFFER_SIZE - 1] = '\0';
    printf("Mensagem recebida: %s\n", buffer);

    char *response = "Mensagem recebida com sucesso!";
    sendto(socket_fd, response, strlen(response), 0,
           (struct sockaddr *)&client_addr, addr_len);
    close(socket_fd);
    return 0;
}