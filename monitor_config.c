#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

// Usamos os sockets dados nas aulas.

/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

str_echo(sockfd) int sockfd;
{
    int n, i;
    char line[MAXLINE];

    // Menu de inicialização da simulação
    system("clear"); // limpar a consola
    printf("Conexão estabelecida com o Simulador!\n");
    printf(" _____________________________________ \n");
    printf("(_____Deseja_Iniciar_a_Simulação?_____)\n");
    printf("(                                     )\n");
    printf("(     y - sim, iniciar a simulação    )\n");
    printf("(     n - não, cancelar a simulação   )\n");
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
            write(sockfd, "INICIAR", 8); // Envia msm para iniciar simulação
            menu = 1;                    // Sai do menu
            break;
        case 'Y':
            printf("\nSimulação Iniciada!\n");
            write(sockfd, "INICIAR", 8); // Envia msm para iniciar simulação
            menu = 1;                    // Sai do menu
            break;
        case 'n':
            printf("\nFechando ligação com o simulador atual!\n");
            write(sockfd, "FECHAR", 7); // Envia msm para cancelar simulação
            close(sockfd);
            printf("\nEsperando nova ligação...\n");
            exit(0);
            break;
        case 'N':
            printf("\nFechando ligação com o simulador atual!\n");
            write(sockfd, "FECHAR", 7); // Envia msm para cancelar simu
            close(sockfd);
            printf("\nEsperando nova ligação...\n");
            exit(0);
            break;
        }
    }
    // Fim do menu de inicialização da simulação

    // Receber dados iniciais das zonas:

    // Monitor com a informação da simulação
    for (;;)
    {
        // Receber mensagem codificada do simulador
        // Descodificar mensagem
        // Mostrar dados no Monitor

        //_____________________AQUI SERÁ FEITO O TRABALHO DA PARTE DO MONITOR_____________________//
        system("clear"); // Limpar Monitor anterior
        printf(" ________________________________________ \n");
        printf("(_______________INFORMAÇÃO_______________)\n");
        printf("( Discoteca: #x       Fila: #y           )\n");
        printf("(                                        )\n");
        printf("( Pista Publica: #x   Fila: #y           )\n");
        printf("(                                        )\n");
        printf("( Zona VIP: #x        Fila: #y           )\n");
        printf("(                                        )\n");
        printf("( WC:#x               Fila: #y           )\n");
        printf("(                                        )\n");
        printf("( Restaurante: #x     Fila: #y           )\n");
        printf("(________________________________________)\n");
        printf("(___________Informação__Global___________)\n");
        printf("( Entraram na Discoteca: #x              )\n");
        printf("( Sairam da discoteca: #y                )\n");
        printf("( Total de VIPs: #z                      )\n");
        printf("(________________________________________)\n");

        /* Lê uma linha do socket */ //<-- Funcionalidade Antiga
        n = readline(sockfd, line, MAXLINE);
        if (n == 0)
            return;
        else if (n < 0)
            err_dump("str_echo: readline error");

        /* Manda linha de volta para o socket. n conta com
           o \0 da string, caso contrário perdia-se sempre
           um caracter!*/
        /* FUNCIONALIDADE ANTIGA, ENVIA DE VOLTA a mesnagem recebida
        if (writen(sockfd, line, n) != n)
            err_dump("str_echo: writen error");
        */
    }
}
