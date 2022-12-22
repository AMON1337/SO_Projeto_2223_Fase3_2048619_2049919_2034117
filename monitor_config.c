#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

//  Usamos os sockets dados nas aulas.

/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

str_echo(sockfd) int sockfd;
{
    int n, i;
    char line[MAXLINE];

    // Menu de inicialização da simulação
    system("clear"); // limpar a consola
    printf("Conexão estabelecida com o simulador!\n");
    printf(" _____________________________________ \n");
    printf("(_____Deseja_Iniciar_a_Simulação?_____)\n");
    printf("(                                     )\n");
    printf("(     y - sim, iniciar a simulação    )\n");
    printf("(     n - não, sair do monitor        )\n");
    printf("(_____________________________________)\n");
    printf("\nInsira uma opção válida: ");
    int menu = 0;

    while (menu == 0)
    {
        char opcao;
        scanf("%c", &opcao);
        switch (opcao)
        {
        case 'y':
            printf("\nSimulação Iniciada!\n");
            menu = 1;
            break;
        case 'Y':
            printf("\nSimulação Iniciada!\n");
            menu = 1;
            break;
        case 'n':
            printf("\nFechando ligação com o simulador atual!\n");
            close(sockfd);
            printf("\nEsperando nova ligação...\n");
            exit(0);
            break;
        case 'N':
            printf("\nFechando ligação com o simulador atual!\n");
            close(sockfd);
            printf("\nEsperando nova ligação...\n");
            exit(0);
            break;
        }
    }
    // Menu de inicialização da simulação

    for (;;)
    {

        //_____________________AQUI SERÁ FEITO O TRABALHO DA PARTE DO MONITOR_____________________//
        printf(" _____________________________________ \n");
        printf("(_______________MONITOR_______________)\n");
        printf("(                                     )\n");
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
