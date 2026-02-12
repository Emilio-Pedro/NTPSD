#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4444
#define BUFFER 1024
double time_double()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9; // conversão para double
};

struct ntp
{
    double t1, t2, t3;
};

int main()
{
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        printf("Erro ao criar socket\n");
        return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Erro ao bindar socket\n");
        return 1;
    }

    printf("Servidor iniciado na porta %d...\n", PORT);

    while (1)
    {
        struct ntp pack;

        ssize_t n = recvfrom(socket_fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            printf("Erro ao receber mensagem\n");
            continue;
        }
        if ((size_t)n != sizeof(pack))
        {
            printf("Mensagem recebida com tamanho inesperado: %zd bytes\n", n);
            continue;
        }
        printf("Mensagem recebida: T1=%.6f", pack.t1);

        pack.t2 = time_double(); // momento de recebimento

        usleep(4000); // Simulação de um atraso de 4ms

        pack.t3 = time_double(); // momento de envio

        sendto(socket_fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client_addr, addr_len);
    }
    close(socket_fd);

    return 0;
}