#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

/* Monitor do tipo socket stream. */

// Variáveis monitor
// Número atual de pessoas na zona(Z), fila da zona(FZ) ou desisténcia da fila da zona (DFZ)

// Z0 - Discoteca
int numZ0 = 0;
int numFZ0 = 0;

// Z1 - Pista de Dança
int numZ1 = 0;
int numFZ1 = 0;
int numDFZ1 = 0; // <--- usado para os logs?

// Z2 - Zona VIP
int numZ2 = 0;
int numFZ2 = 0;
int explusoVIP = 0;

// Z3 - WC
int numZ3 = 0;
int numFZ3 = 0;

// Z4 - Restaurante
int numZ4 = 0;
//int numFZ4 = 0; //Não há fila

// Estado da Discoteca
char estadoDisco[8] = "FECHADO"; // Aberto ou Fechado, começa simulação a fechado
// Total de pessoas que já entraram na discoteca
int totalDisco = 0;
// Total de pessoas que saíram da discoteca
int saidaDisco = 0;
// Número de pessoas VIP que entraram da discoteca
int totalVIPS = 0;
// Número de pessoas expulsas do discoteca
int totalExpulsos = 0;
// Número de pessoas que desistiram da discoteca
int totalDesistencias = 0;

// Variáveis (informação) do cliente, recebidas do simulador
int id_cliente;
int acontecimento;
int tempo;
int vip;

str_echo(sockfd) int sockfd;
{
    int n, i;
    char line[MAXLINE + 1]; // Linha de testo recebida com o acontecimento + info cliente

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
            write(sockfd, "FECHAR", 7); // Envia msm para cancelar simulação
            close(sockfd);
            printf("\nEsperando nova ligação...\n");
            exit(0);
            break;
        }
    }
    // Fim do menu de inicialização da simulação

    // Monitor com a informação da simulação
    for (;;)
    {
        // Receber mensagem codificada do simulador,
        // Descodificar mensagem,
        // Mostrar dados no Monitor

        //_____________________AQUI SERÁ FEITO O TRABALHO DA PARTE DO MONITOR_____________________//

        system("clear"); // Limpar Monitor anterior
        printf(" ________________________________________ \n");
        printf("(_______________INFORMAÇÃO_______________)\n");
        printf("                                          \n");
        printf("  Discoteca: %d       Fila: %d            \n", numZ0, numFZ0);
        printf("                                          \n");
        printf("  Pista de Dança: %d  Fila: %d            \n", numZ1, numFZ1);
        printf("                                          \n");
        printf("  Zona VIP: %d        Fila: %d            \n", numZ2, numFZ2);
        printf("                                          \n");
        printf("  WC: %d              Fila: %d            \n", numZ3, numFZ3);
        printf("                                          \n");
        printf("  Restaurante: %d     Sem Fila            \n", numZ4/*, numFZ4*/);
        printf(" ________________________________________ \n");
        printf("(___________Informação__Global___________)\n");
        printf("                                          \n");
        printf("  Estado da Discoteca: %s                 \n", estadoDisco);
        printf("  Entraram na Discoteca: %d               \n", totalDisco);
        printf("  Sairam da discoteca: %d                 \n", saidaDisco);
        printf("  Total de VIPs: %d                       \n", totalVIPS);
        printf("  Total de desistências das filas: %d     \n", totalDesistencias);
        printf("  Explusos da Discoteca: %d               \n", totalExpulsos);
        printf(" ________________________________________ \n");
        printf("(________________________________________)\n");
        printf("(ExpulsoVIP %d___________________________)\n", explusoVIP); //<-- TESTE, APAGAr

        /* Lê uma linha do socket */ // Linha enviada do simulador
        bzero(line, MAXLINE);        // Limpar Buffer
        n = readline(sockfd, line, MAXLINE);
        if (n == 0)
            return;
        else if (n < 0)
            err_dump("str_echo: readline error");

        // Descodificar a linha recebida e guardar os valores temporáriamente em variáveis
        sscanf(line, "%d.%d.%d.%d", &id_cliente, &acontecimento, &tempo, &vip);

        // Acontecimentos da Discoteca:
        switch (acontecimento)
        {
        case 60: // Abertura da Discoteca
            estadoDisco[8] = '/0';
            strcpy(estadoDisco, "ABERTO");
            break;
        case 69: // Encerramento da Discoteca
            estadoDisco[8] = '/0';
            strcpy(estadoDisco, "FECHADO");
            break;
        case 00: // Entrada - Discoteca
            numFZ0--;
            numZ0++;
            totalDisco++;
            if (vip == 1) // Se o cliente que entrou na Discoteca é VIP
                totalVIPS++;
            break;
        case 01: // Entrada - Pista de Dança
            numFZ1--;
            numZ1++;
            break;
        case 02: // Entrada - Zona VIP
            numFZ2--;
            numZ2++;
            break;
        case 03: // Entrada - WC
            numFZ3--;
            numZ3++;
            break;
        case 04: // Entrada - Restaurante
            //numFZ4--;
            numZ4++;
            break;
        case 10: // Espera na fila - Discoteca
            numFZ0++;
            break;
        case 11: // Espera na fila - Pista de dança
            numFZ1++;
            break;
        case 12: // Espera na fila - Zona VIP
            numFZ2++;
            break;
        case 13: // Espera na fila - WC
            numFZ3++;
            break;
        case 20: // Desistência da fila - Discoteca
            numFZ0--;
            totalDesistencias++;
            break;
        case 21: // Desistência da fila - Pista de Dança
            numFZ1--;
            numDFZ1++;
            totalDesistencias++;
            break;
        case 22: // Desistência da fila - Zona VIP
            numFZ2--;
            totalDesistencias++;
            break;
        case 23: // Desistência da fila - WC
            numFZ3--;
            totalDesistencias++;
            break;
        case 30: // Saída - Discoteca
            numZ0--;
            saidaDisco++;
            break;
        case 31: // Saída - Pista de Dança
            numZ1--;
            break;
        case 32: // Saída - Zona VIP
            numZ2--;
            break;
        case 33: // Saída - WC
            numZ3--;
            break;
        case 34: // Saída - Restaurante
            numZ4--;
            break;
        case 38: // Saída - Expulso da Zona VIP
            explusoVIP++;
            break;
        case 39: // Saída - Expulso da Discoteca
            numZ0--;
            saidaDisco++;
            totalExpulsos++;
            break;
        default: // Outro Acontecimento // ERRO
            printf("ERRO: Acontecimento Inválido! \n");
            break;
        }

        /* Manda linha de volta para o socket. n conta com
           o \0 da string, caso contrário perdia-se sempre
           um caracter!*/
        /* FUNCIONALIDADE ANTIGA, ENVIA DE VOLTA a mensagem recebida
        if (writen(sockfd, line, n) != n)
            err_dump("str_echo: writen error");
        */
    }
}
