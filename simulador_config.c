#include "unix.h" //Tem as bibliotecas comuns do projeto
#include <stdio.h>
#define MAXLINE 512

// Ojetivo será enviar cada acontecimento codificado numa linha só,
// No monitor será descodificado e mostrado.

// Envia a mensagem, é o único objetivo
str_cli(sendline, sockfd) char sendline[MAXLINE];
int sockfd;
{
    int n;

    // Envia o acontecimento codificado para o monitor
    n = strlen(sendline);
    if (write(sockfd, sendline, n) != n)
        err_dump("str_cli: writen error on socket");
}
