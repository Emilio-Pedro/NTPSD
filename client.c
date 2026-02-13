#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4444
#define BUFFER 1024
#define IP_SERVIDOR "127.0.0.1"
struct ntp
{
    double t1, t2, t3;
};

double time_double()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9; // conversão para double
};
double relogio_client(double offset_falso)
{
    return time_double() + offset_falso; // simula o envio do tempo com um offset
};

/*RTT = T4-T1
OFFSET = T2 + RTT/2 -T4
t1 = cliente envia;
t2 = servidor recebe;
t3 = servidor envia;
t4 = cliente recebe;
*/
int main(void)
{
    int socket_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        printf("Erro ao criar socket\n");
        return 1;
    }
    memset(&server_addr, 0, sizeof(server_addr)); // zerar os valores da struct para evitar lixo de memoria
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP_SERVIDOR, &server_addr.sin_addr) != 1)
    {
        printf("Endereço de IP de servidor invalido %s\n", IP_SERVIDOR);
        close(socket_fd);
        return 1;
    }

    struct ntp pack;
    double testeoffset = -5;
    pack.t1 = relogio_client(testeoffset); // simula o envio do tempo com um offset
    pack.t2 = 0;
    pack.t3 = 0;

    // char *msgteste = "me retorne o horario atual";
    if (sendto(socket_fd, &pack, sizeof(pack), 0, (struct sockaddr *)&server_addr, addr_len) < 0)
    {
        printf("Erro ao enviar mensagem\n");
        close(socket_fd);
        return 1;
    }

    printf("Pedido enviado:\n");
    ssize_t n = recvfrom(socket_fd, &pack, sizeof(pack), 0, NULL, NULL); /// ja sabemos que o servidor que vai responder
    if (n < 0)
    {
        printf("Erro ao receber mensagem\n");
        close(socket_fd);
        return 1;
    }
    if ((size_t)n != sizeof(pack))
    {
        printf("Mensagem recebida com tamanho inesperado: %zd bytes\n", n);
        close(socket_fd);
        return 1;
    }

    double t4 = relogio_client(testeoffset);
    printf("Resposta recebida:\n");
    /*printf("T2=%.9f\n", pack.t2);
    printf("T3=%.9f\n", pack.t3);
    printf("T4(do cliente)=%.6f\n", t4);
    */
    /* considerando t2 = t3
    double rtt = t4 - pack.t1;
    double offset = pack.t2 + rtt / 2 - t4;
    double delay = rtt / 2;
*/
    double offset = ((pack.t2 - pack.t1) + (pack.t3 - t4)) / 2.0;
    double delay = ((t4 - pack.t1) - (pack.t3 - pack.t2));

    double hora_local = t4; // hora local sem ajuste
    double hora_corrigida = hora_local + offset;
    printf("Offset calculado: %.9f segundos\n", offset);
    printf("Delay calculado: %.9f segundos\n", delay);

    time_t agora = (time_t)hora_local;
    printf("Hora local errada(cliente): %s", ctime(&agora));           // data/hora padronizada
    printf("Hora local errada(cliente): %.9f segundos\n", hora_local); // data/hora em segundos
    printf("Hora corrigida: %.9f segundos\n", hora_corrigida);         // data/hora corrigida em segundos
    time_t data_hora = (time_t)hora_corrigida;
    printf("Hora local formatada: %s", ctime(&data_hora)); // data/hora corrigida padronizada

    close(socket_fd);

    return 0;
}