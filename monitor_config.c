#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

//  Usamos os sockets dados nas aulas.

/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

str_echo(sockfd) int sockfd;
{
    int n, i;
    char line[MAXLINE];

    for (;;)
    {

        //_____________________AQUI SERÁ FEITO O TRABALHO DA PARTE DO MONITOR_____________________//
        printf(" _____________________________________ \n");
        printf("(_______________MONITOR_______________)\n");
        printf("(             mostrar dados           )\n");
        printf("(       tratar acontecimentos         )\n");
        printf("(_____________________________________)\n");
        /* Lê uma linha do socket */

        n = readline(sockfd, line, MAXLINE);
        if (n == 0)
            return;
        else if (n < 0)
            err_dump("str_echo: readline error");

        /* Manda linha de volta para o socket. n conta com
           o \0 da string, caso contrário perdia-se sempre
           um caracter! */
        if (writen(sockfd, line, n) != n)
            err_dump("str_echo: writen error");
    }
}
