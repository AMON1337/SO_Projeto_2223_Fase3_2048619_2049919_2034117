#include "unix.h" //Tem as bibliotecas comuns do projeto
#define MAXLINE 512

/* Monitor do tipo socket stream. */

// Variáveis monitor
// Número atual de pessoas na zona(Z), fila da zona(FZ) ou desisténcia da fila da zona (DFZ)
// TZ - número total de pessoas na zona

// Z0 - Discoteca
int numZ0 = 0;
int numFZ0 = 0;
int numDFZ0 = 0; // log

// Z1 - Pista de Dança
int numZ1 = 0;
int numFZ1 = 0;
int numDFZ1 = 0; // log
int numTZ1 = 0;  // log
int numFCZ1 = 0; // log
// Z2 - Zona VIP
int numZ2 = 0;
int numFZ2 = 0;
int numDFZ2 = 0;     // log
int numTZ2 = 0;      // log
int numFCZ2 = 0;     // log
int expulsosVIP = 0; // log

// Z3 - WC
int numZ3 = 0;
int numFZ3 = 0;
int numDFZ3 = 0; // log
int numTZ3 = 0;  // log
int numFCZ3 = 0; // log

// Z4 - Restaurante
int numZ4 = 0;
int numFZ4 = 0;  // Buffet
int numTZ4 = 0;  // log
int numTFZ4 = 0; // log
int numFCZ4 = 0; // log

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

// Váriáveis para o log
int tempoDiscoAbriu = 0;      // log
int tempoDiscoFechou = 0;     // log
int tempoDiscoRealFechou = 0; // log

// Variáveis (informação) do cliente, recebidas do simulador
int id_cliente;
int acontecimento;
int tempo;
int vip;

// Função para  escrever os Logs da Discoteca num ficheiros!
// void logMonitor(int totalDisco, int saidaDisco, int totalVIPS, int totalDesistencias, int totalExpulsos)
void logMonitor()
{
    // Escreve o Acontecimento no logMonitor.log
    FILE *fpLog = fopen("logMonitor.log", "w+"); // Abrir e/ou criar o ficheiro logs para escrever
    if (fpLog == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro de logs!\nImpossível continuar!\n");
        return;
    }
    fprintf(fpLog, "!------------------------ LOGS DO MONITOR ----------------------!\n\n");
    fprintf(fpLog, " Clientes/Tarefas criados <%d>                                   \n", totalDisco + numDFZ0);
    fprintf(fpLog, "                                                                 \n");
    fprintf(fpLog, "                      ->Informação Global<-                      \n");
    fprintf(fpLog, " Entraram na Discoteca: %d                                       \n", totalDisco);
    fprintf(fpLog, " Sairam da Discoteca: %d                                         \n", saidaDisco);
    fprintf(fpLog, " Total de VIPs: %d                                               \n", totalVIPS);
    fprintf(fpLog, " Total de Desistências das Filas: %d                             \n", totalDesistencias);
    fprintf(fpLog, " Expulsos da Discoteca: %d                                       \n", totalExpulsos);
    fprintf(fpLog, " Expulsos da Zona VIP: %d                                        \n", expulsosVIP);
    fprintf(fpLog, "                                                                 \n");
    fprintf(fpLog, "          ->Número de clientes que entraram nas Zonas<-          \n");
    fprintf(fpLog, " Zona 1 - Pista de Dança: %d                                     \n", numTZ1);
    fprintf(fpLog, " Zona 2 - Zona VIP: %d                                           \n", numTZ2);
    fprintf(fpLog, " Zona 3 - WC: %d                                                 \n", numTZ3);
    fprintf(fpLog, " Zona 4 - Restaurante: %d                                        \n", numTZ4);
    fprintf(fpLog, " Zona 4 - Restaurante-Buffet: %d                                 \n", numTFZ4);
    fprintf(fpLog, "                                                                 \n");
    fprintf(fpLog, "               ->Número de Desistências nas Filas<-              \n");
    fprintf(fpLog, " Desistiram da Fila da Discoteca: %d                             \n", numDFZ0);
    fprintf(fpLog, " Desistiram da Fila da Pista de Dança: %d                        \n", numDFZ1);
    fprintf(fpLog, " Desistiram da Fila da Zona VIP: %d                              \n", numDFZ2);
    fprintf(fpLog, " Desistiram da Fila da WC: %d                                    \n", numDFZ3);
    fprintf(fpLog, "                                                                 \n");
    fprintf(fpLog, " ->Número de vezes que os clientes encontraram as Filas cheias<- \n");
    fprintf(fpLog, " Zona 1 - Pista de Dança: %d                                     \n", numFCZ1);
    fprintf(fpLog, " Zona 2 - Zona VIP: %d                                           \n", numFCZ2);
    fprintf(fpLog, " Zona 3 - WC: %d                                                 \n", numFCZ3);
    fprintf(fpLog, " Zona 4 - Restaurante: %d                                        \n", numFCZ4);
    fprintf(fpLog, "                                                                 \n");
    fprintf(fpLog, "            ->Informação sobre o tempo da Simulação<-            \n");
    fprintf(fpLog, " Discoteca aberta durante: %d segundos                           \n", tempoDiscoFechou - tempoDiscoAbriu);
    fprintf(fpLog, " Último cliente saiu %d segundos depois da Discoteca abrir       \n", tempoDiscoRealFechou - tempoDiscoAbriu);
    fprintf(fpLog, " Discoteca ficou vazia %d segundos depois de fechar              \n", tempoDiscoRealFechou - tempoDiscoFechou);
    fprintf(fpLog, "!---------------------------------------------------------------!\n\n");
    fclose(fpLog);
}

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

    // Monitor com a informação da simulação - atualiza o monitor cada vez que receber uma mensagem
    for (;;)
    {
        /* Ordem de tratamento de uma mensagem */
        // Receber mensagem codificada do simulador,
        // Descodificar mensagem, para saber o que aconteceu,
        // Mostrar dados no Monitor

        /* Lê uma linha do socket */         // Linha enviada do simulador
        bzero(line, MAXLINE);                // Limpar Buffer
        n = readline(sockfd, line, MAXLINE); // Receber mensagem codificada do simulador
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
            estadoDisco[8] = '\0';
            strcpy(estadoDisco, "ABERTO");
            tempoDiscoAbriu = tempo; // tempo recebido
            break;
        case 69: // Encerramento da Discoteca
            estadoDisco[8] = '\0';
            strcpy(estadoDisco, "FECHADO");
            tempoDiscoFechou = tempo; // tempo recebido
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
            numTZ1++;
            break;
        case 02: // Entrada - Zona VIP
            numFZ2--;
            numZ2++;
            numTZ2++;
            break;
        case 03: // Entrada - WC
            numFZ3--;
            numZ3++;
            numTZ3++;
            break;
        case 04: // Entrada - Restaurante
            numZ4++;
            numTZ4++;
            break;
        case 05:       // Entrada - Restaurante-Buffet
            numFZ4++;  // Buffet
            numTFZ4++; // Buffet
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
            numDFZ0++;
            totalDesistencias++;
            break;
        case 21: // Desistência da fila - Pista de Dança
            numFZ1--;
            numDFZ1++;
            totalDesistencias++;
            break;
        case 22: // Desistência da fila - Zona VIP
            numFZ2--;
            numDFZ2++;
            totalDesistencias++;
            break;
        case 23: // Desistência da fila - WC
            numFZ3--;
            numDFZ3++;
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
        case 35:      // Saída - Restaurane-Buffet
            numFZ4--; // Buffet
            break;
        case 38: // Saída - Expulso da Zona VIP
            expulsosVIP++;
            break;
        case 39: // Saída - Expulso da Discoteca
            numZ0--;
            saidaDisco++;
            totalExpulsos++;
            break;
        case 41: // Fila cheia - Pista de Dança
            numFCZ1++;
            break;
        case 42: // Fila cheia - Zona VIP
            numFCZ2++;
            break;
        case 43: // Fila cheia - WC
            numFCZ3++;
            break;
        case 44: // Fila cheia - Restaurante
            numFCZ4++;
            break;
        default: // Outro Acontecimento // ERRO
            printf("ERRO: Acontecimento Inválido! \n");
            break;
        }

        /*/------------------! MONITOR !------------------/*/
        system("clear"); // Limpar Monitor anterior
        printf(" ________________________________________ \n");
        printf("(_______________INFORMAÇÃO_______________)\n");
        printf("                                          \n");
        printf("  Discoteca: %d       Fila: %d            \n", numZ0, numFZ0);
        printf(" ________________________________________ \n");
        printf("                                          \n");
        printf("  Pista de Dança: %d  Fila: %d            \n", numZ1, numFZ1);
        printf("                                          \n");
        printf("  Zona VIP: %d        Fila: %d            \n", numZ2, numFZ2);
        printf("                                          \n");
        printf("  WC: %d              Fila: %d            \n", numZ3, numFZ3);
        printf("                                          \n");
        printf("  Restaurante: %d     Buffet: %d          \n", numZ4, numFZ4);
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
        /*/------------------! MONITOR !------------------/*/

        /* Verifica se a simulação já acabou, ou seja, todos os clientes já sairam da Discoteca */
        // Imprime info da discoteca depois dela ter fechado para o logMonitor.log
        if (numZ0 == 0 && numFZ0 == 0)
        {
            tempoDiscoRealFechou = tempo; // tempo que saiu o ultimo cliente
            logMonitor();
            printf("\nSimulação acabou.\n");
            printf("Logs do Monitor criadas!\n");

            close(sockfd);
            break;
        }
    }
}
