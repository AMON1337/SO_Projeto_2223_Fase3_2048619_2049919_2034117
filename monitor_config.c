#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

// Usamos os sockets dados nas aulas
/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

//Variáveis monitor
//Número atual de pessoas na zona(Z) ou na fila da zona(FZ)

//Z0 - Discoteca
int numZ0 = 0;
int numFZ0 = 0;

//Z1 - Pista pública
int numZ1 = 0;
int numFZ1 = 0;

//Z2 - Zona VIP
int numZ2 = 0;
int numFZ2 = 0;

//Z3 - WC
int numZ3 = 0;
int numFZ3 = 0;

//Z4 - Restaurante
int numZ4 = 0;
int numFZ4 = 0;

//Total de pessoas que já entraram na discoteca
int totalDisco;
//Total de pessoas que saíram da discoteca
int saidaDisco;
//Número de pessoas VIP dentro da discoteca
int totalVIPS;

//Variáveis simulador
int Acontecimento;


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
        printf("( Discoteca: %d       Fila: %d           )\n",numZ0,numFZ0);
        printf("(                                        )\n");
        printf("( Pista Publica: %d   Fila: %d           )\n",numZ1,numFZ1);
        printf("(                                        )\n");
        printf("( Zona VIP: %d        Fila: %d           )\n",numZ2,numFZ2);
        printf("(                                        )\n");
        printf("( WC: %d              Fila: %d           )\n",numZ3,numFZ3);
        printf("(                                        )\n");
        printf("( Restaurante: %d     Fila: %d           )\n",numZ4,numFZ4);
        printf("(________________________________________)\n");
        printf("(___________Informação__Global___________)\n");
        printf("( Entraram na Discoteca: %d              )\n",totalDisco);
        printf("( Sairam da discoteca: %d                )\n",saidaDisco);
        printf("( Total de VIPs: %d                      )\n",totalVIPS);
        printf("(________________________________________)\n");

        /* Lê uma linha do socket */ //<-- Funcionalidade Antiga
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

        switch (Acontecimento)
        {
        // Abertura da Discoteca
        case 60:
            break;
        // Encerramento da Discoteca
        case 69:
            return;
            break;
        // Entrada para a Discoteca
        case 00:
            numFZ0--;
            numZ0++;
            totalDisco++;
            break;
        // Entrada pista de dança
        case 01:
            numFZ1--;
            numZ1++;
            break;
        // Entrada zona VIP
        case 02:
            numFZ2--;
            numZ2++;
            break;
        // Entrada na WC
        case 03:
            numFZ3--;
            numZ3++;    
            break;
        // Entrada no restaurante
        case 04:
            numFZ4--;
            numZ4++;
            break;
        // Espera na fila - Discoteca
        case 10:
            numZ0++;
            break;
        // Espera na fila - Pista de dança
        case 11:
            numZ1++;
            break;
        // Espera na fila - Zona VIP
        case 12:
            numZ2++;
            break;
        // Espera na fila - WC
        case 13:
            numZ3++;
            break;
        // Desistência da fila - Discoteca
        case 20:
            numFZ0--;
            break;
        // Desistência da fila - Pista de Dança
        case 21:
            numFZ1--;
            break;
        // Desistência da fila - Zona VIP
        case 22:
            numFZ2--;
            break;
        // Desistência da fila - WC
        case 23:
            numFZ3--;
            break;
        // Saída - Discoteca
        case 30:
            numZ0--;
            saidaDisco++;
            break;
        // Saída - Pista de Dança
        case 31:
            numZ1--;
            break;
        // Saída - Zona VIP
        case 32:
            numZ2--;
            break;
        // Saída- WC
        case 33:
            numZ3--;
            break;
        // Saída - restaurante
        case 34:
            numZ4--;
            break;
        default:
            printf("ERRO: Acontecimento inválido.\n");
            break;
        }
    }
}

