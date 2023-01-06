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

// Funcionamento Antigo  --- Apagar no fim!

/* Cliente do tipo socket stream.
   Lê string de fp e envia para sockfd.
   Lê string de sockfd e envia para stdout */
/*
str_cli(fp, sockfd)
    FILE *fp;
int sockfd;
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (fgets(sendline, MAXLINE, fp) != NULL)
    {

        // Envia string para sockfd. Note-se que o \0 não é enviado

        n = strlen(sendline);
        if (writen(sockfd, sendline, n) != n)
            err_dump("str_cli: writen error on socket");

        // Tenta ler string de sockfd. Note-se que tem de
        //   terminar a string com \0

        n = readline(sockfd, recvline, MAXLINE);
        if (n < 0)
            err_dump("str_cli:readline error");
        recvline[n] = 0;

        // Envia a string para stdout

        fputs(recvline, stdout);
    }

    if (ferror(fp))
        err_dump("str_cli: error reading file");
}
*/
