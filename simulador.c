#include "unix.h"      //Tem as bibliotecas comuns do projeto
#include <pthread.h>   //Usar tarefas para simular uma pessoa
#include <time.h>      //Tempo da simulação
#include <semaphore.h> //Usar semaforos

// Comunicação com o Monitor
#define MAXLINE 512 // Tamanho da Mensagem

// Estruturas de dados
typedef struct cliente
{                      // tarefa = cliente
    int id_cliente;    // ID do cliente
    int acontecimento; // O que esta a fazer agora
    int tempo;         // Ainda não sei como implementar
    int vip;           // 1 - VIP | 0 - Não é VIP
};

typedef struct discoteca
{
    int n_zonas;
    char z0_nome[15];
    char z1_nome[15];
    char z2_nome[15];
    char z3_nome[15];
    char z4_nome[15];
    int z0_max;
    int z1_max;
    int z2_max;
    int z3_max;
    int z4_max;
    int z0_fila_max;
    int z1_fila_max;
    int z2_fila_max;
    int z3_fila_max;
    int z4_fila_max;
    int prob_desistir_fila;
    int prob_ser_vip;
    int prob_ser_expulso;
};

struct discoteca disco; // Estado interno da nossa DISCOTECA

struct cliente thread_array[500]; // Serão criados 500 tarefas/clientes

pthread_t threads[500]; // Serão criados 500 tarefas/clientes

int capacidadeDisco = 500; // Utilizado para criar tarefas no int main();

int inciciarSimulacao = 0; // 0 - Espera pelo monitor iniciar, 1 - O monitor deu sinal iniciar

void lerConfigInicial()
{
    char nome[MAXLINE];
    char valor[MAXLINE];

    printf("**Loading das configurações inciais**\n");

    FILE *fpConfig = fopen("simulador_config_inicial.txt", "r"); // Abrir para ler o ficheiro de configuração incial da discoteca

    if (fpConfig == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro com a configuração incicial!\nImpossível continuar!\n");

        return;
    }

    // rewind(fpConfig); // Volta ao inicio do ficheiro para nova leitura
    //  Obter as variáveis
    while (fscanf(fpConfig, "%s : %s", nome, valor) != EOF)
    {
        if (strcmp(nome, "n_zonas") == 0) // Números de zonas da discoteca
            disco.n_zonas = atoi(valor);  // atoi converte str para int
        if (strcmp(nome, "z0_nome") == 0) // Zonas da discoteca
            strcpy(disco.z0_nome, valor); // Copia string para o struct
        if (strcmp(nome, "z1_nome") == 0)
            strcpy(disco.z1_nome, valor); // String
        if (strcmp(nome, "z2_nome") == 0)
            strcpy(disco.z2_nome, valor); // String
        if (strcmp(nome, "z3_nome") == 0)
            strcpy(disco.z3_nome, valor); // String
        if (strcmp(nome, "z4_nome") == 0)
            strcpy(disco.z4_nome, valor); // String
        if (strcmp(nome, "z0_max") == 0)  // Capacidade das zonas
            disco.z0_max = atoi(valor);
        if (strcmp(nome, "z1_max") == 0)
            disco.z1_max = atoi(valor);
        if (strcmp(nome, "z2_max") == 0)
            disco.z2_max = atoi(valor);
        if (strcmp(nome, "z3_max") == 0)
            disco.z3_max = atoi(valor);
        if (strcmp(nome, "z4_max") == 0)
            disco.z4_max = atoi(valor);
        if (strcmp(nome, "z0_fila_max") == 0) // Capacidade máxima da fila das zonas
            disco.z0_fila_max = atoi(valor);
        if (strcmp(nome, "z1_fila_max") == 0)
            disco.z1_fila_max = atoi(valor);
        if (strcmp(nome, "z2_fila_max") == 0)
            disco.z2_fila_max = atoi(valor);
        if (strcmp(nome, "z3_fila_max") == 0)
            disco.z3_fila_max = atoi(valor);
        if (strcmp(nome, "z4_fila_max") == 0)
            disco.z4_fila_max = atoi(valor);
        if (strcmp(nome, "prob_desistir_fila") == 0) // Probabilidades
            disco.prob_desistir_fila = atoi(valor);
        if (strcmp(nome, "prob_ser_vip") == 0)
            disco.prob_ser_vip = atoi(valor);
        if (strcmp(nome, "prob_ser_expulso") == 0)
            disco.prob_ser_expulso = atoi(valor);
    }
    printf("**Loading Completo**\n\n");
    fclose(fpConfig);
}

void logInicialDisco() // Escreve no ficheiro de LOGS da simulação
{
    FILE *fpLog = fopen("logSimulador.log", "w"); // Abrir e/ou criar o ficheiro logs para escrever

    if (fpLog == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro de logs!\nImpossível continuar!\n");
        return;
    }
    else
    {
        fprintf(fpLog, "!------ LOGS DA SIMULAÇÃO ------!\n\n"); // escreve no ficheiro de logs do simulador
        fprintf(fpLog, "**Configuração Inicial da Discoteca**\n");
    }

    // Escreves nos logs do simulador os dados internos inciais da simulação
    fprintf(fpLog, "Numero de zonas na discoteca: %d\n", disco.n_zonas);
    fprintf(fpLog, "Nome da zona 0: %s\n", disco.z0_nome); // Nomes das zonas
    fprintf(fpLog, "Nome da zona 1: %s\n", disco.z1_nome);
    fprintf(fpLog, "Nome da zona 2: %s\n", disco.z2_nome);
    fprintf(fpLog, "Nome da zona 3: %s\n", disco.z3_nome);
    fprintf(fpLog, "Nome da zona 4: %s\n", disco.z4_nome);

    fprintf(fpLog, "\nCapacidades máximas das zonas\n"); // Capacidade máxima
    fprintf(fpLog, "Capacidade máxima da zona 0: %d\n", disco.z0_max);
    fprintf(fpLog, "Capacidade máxima da zona 1: %d\n", disco.z1_max);
    fprintf(fpLog, "Capacidade máxima da zona 2: %d\n", disco.z2_max);
    fprintf(fpLog, "Capacidade máxima da zona 3: %d\n", disco.z3_max);
    fprintf(fpLog, "Capacidade máxima da zona 4: %d\n", disco.z4_max);

    fprintf(fpLog, "\nCapacidades máximas das filas das zonas\n"); // Capacidade máxima das filas
    fprintf(fpLog, "Capacidade máxima da fila da zona 0: %d\n", disco.z0_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 1: %d\n", disco.z1_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 2: %d\n", disco.z2_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 3: %d\n", disco.z3_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 4: %d\n", disco.z4_fila_max);

    fprintf(fpLog, "\nProbabilidades da simulação\n"); // Capacidade máxima das filas
    fprintf(fpLog, "Probabilidade de desistir da fila: %d\n", disco.prob_desistir_fila);
    fprintf(fpLog, "Probabilidade de ser VIP: %d\n", disco.prob_ser_vip);
    fprintf(fpLog, "Probabilidade de ser expluso: %d\n", disco.prob_ser_expulso);

    // Acontecimentos da simulação
    fprintf(fpLog, "\n**Acontecimentos da simulação**\n"
                   "60 : Abertura da Discoteca\n"
                   "69 : Encerramento da Discoteca\n\n"
                   "00 : Entrada para a Discoteca\n"
                   "01 : Entrada pista de dança\n"
                   "02 : Entrada zona VIP\n"
                   "03 : Entrada na WC\n"
                   "04 : Entrada no restaurante\n\n"
                   "10 : Espera na fila - Discoteca\n"
                   "11 : Espera na fila - Pista de dança\n"
                   "12 : Espera na fila - Zona VIP\n"
                   "13 : Espera na fila - WC\n\n"
                   "20 : Desistência da fila - Discoteca\n"
                   "21 : Desistência da fila - Pista de Dança\n"
                   "22 : Desistência da fila - Zona VIP\n"
                   "23 : Desistência da fila - WC\n\n"
                   "30 : Saída - Discoteca\n"
                   "31 : Saída - Pista de Dança\n"
                   "32 : Saída - Zona VIP\n"
                   "33 : Saída- WC\n"
                   "34 : Saída - restaurante\n");

    fclose(fpLog);
}

void printInicialDisco()
{ // Faz um print do estado inicial da discoteca no ecrã da simulação
    printf("**Configuração Inicial da Discoteca**\n");

    printf("Numero de zonas na discoteca: %d\n", disco.n_zonas);
    printf("Nome da zona 0: %s\n", disco.z0_nome); // Nomes das zonas
    printf("Nome da zona 1: %s\n", disco.z1_nome);
    printf("Nome da zona 2: %s\n", disco.z2_nome);
    printf("Nome da zona 3: %s\n", disco.z3_nome);
    printf("Nome da zona 4: %s\n", disco.z4_nome);

    printf("\nCapacidades máximas das zonas\n"); // Capacidade máxima
    printf("Capacidade máxima da zona 0: %d\n", disco.z0_max);
    printf("Capacidade máxima da zona 1: %d\n", disco.z1_max);
    printf("Capacidade máxima da zona 2: %d\n", disco.z2_max);
    printf("Capacidade máxima da zona 3: %d\n", disco.z3_max);
    printf("Capacidade máxima da zona 4: %d\n", disco.z4_max);

    printf("\nCapacidades máximas das filas das zonas\n"); // Capacidade máxima das filas
    printf("Capacidade máxima da fila da zona 0: %d\n", disco.z0_fila_max);
    printf("Capacidade máxima da fila da zona 1: %d\n", disco.z1_fila_max);
    printf("Capacidade máxima da fila da zona 2: %d\n", disco.z2_fila_max);
    printf("Capacidade máxima da fila da zona 3: %d\n", disco.z3_fila_max);
    printf("Capacidade máxima da fila da zona 4: %d\n", disco.z4_fila_max);

    printf("\nProbabilidades da simulação\n"); // Capacidade máxima das filas
    printf("Probabilidade de desistir da fila: %d\n", disco.prob_desistir_fila);
    printf("Probabilidade de ser VIP: %d\n", disco.prob_ser_vip);
    printf("Probabilidade de ser expluso: %d\n", disco.prob_ser_expulso);

    // Acontecimentos da simulação
    printf("\n**Acontecimentos da simulação**\n"
           "60 : Abertura da Discoteca\n"
           "69 : Encerramento da Discoteca\n\n"
           "00 : Entrada para a Discoteca\n"
           "01 : Entrada pista de dança\n"
           "02 : Entrada zona VIP\n"
           "03 : Entrada na WC\n"
           "04 : Entrada no restaurante\n\n"
           "10 : Espera na fila - Discoteca\n"
           "11 : Espera na fila - Pista de dança\n"
           "12 : Espera na fila - Zona VIP\n"
           "13 : Espera na fila - WC\n\n"
           "20 : Desistência da fila - Discoteca\n"
           "21 : Desistência da fila - Pista de Dança\n"
           "22 : Desistência da fila - Zona VIP\n"
           "23 : Desistência da fila - WC\n\n"
           "30 : Saída - Discoteca\n"
           "31 : Saída - Pista de Dança\n"
           "32 : Saída - Zona VIP\n"
           "33 : Saída- WC\n"
           "34 : Saída - restaurante\n");
}

void *rotinaCliente(void *ptr)
{
    // Identificar o cliente;
    struct cliente *cliente;
    cliente = (struct cliente *)ptr;

    // Probabilidade de ser VIP
    cliente->vip = rand() % disco.prob_ser_vip;
    if (rand() % 100 + 1 <= disco.prob_ser_vip) // random de 1-100 for <= prob_ser_vip, então é vip
        cliente->vip = 1;                       // VIP
    else
        cliente->vip = 0; // NO VIP
    printf("Iniciei rotina, Sou o clinete de ID: %d,VIP:%d\n", cliente->id_cliente, cliente->vip);
}

int main(void)
{
    int sockfd, servlen;
    struct sockaddr_un serv_addr;

    /* Cria socket stream */

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_dump("simulador: can't open stream socket");

    /* Primeiro uma limpeza preventiva!
       Dados para o socket stream: tipo + nome do ficheiro.
         O ficheiro identifica o servidor */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

    /* Tenta estabelecer uma ligação. Só funciona se o servidor tiver
         sido lançado primeiro (o servidor tem de criar o ficheiro e associar
         o socket ao ficheiro) */

    if (connect(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
        err_dump("O Simulador não consegue ligar-se ao Monitor!\n");

    /*______________________AQUI COMEÇA O NOSSO TRABALHO______________________*/

    printf("Simulador ONLINE\n\n"); //

    // Esperar iniciar simulação
    printf("Esperando para començar a simulação...\n");
    while (inciciarSimulacao == 0)
    {
    }

    lerConfigInicial(); // Lê do ficheiro incial a configuração inicial

    logInicialDisco();   // LOGS - Escreve nos logs do simulador o estado incial da discoteca
    printInicialDisco(); // ECRÂ SIMULAÇÂO - Print do estado incial da discoteca

    // Inicializar os Semáforos

    // Criação das tarefas
    int i = 0;
    for (i = 0; i < capacidadeDisco; i++) // capacidadeDisco é de 500
    {
        usleep(1000000); // generates 1 second delay
        if (pthread_create(&(threads[i]), NULL, *rotinaCliente, &thread_array[i]) != 0)
        {
            printf("Erro ao criar o cliente com o id:%d", i);
            return 0;
        }
        thread_array[i].id_cliente = i + 1; // i=0, id=1
    }

    // pthread_cancel() para fechar os threads?

    /*______________________AQUI ACABA O NOSSO TABALHO______________________*/

    /* Envia as linhas lidas do teclado para o socket */
    str_cli(stdin, sockfd); // Socket
    /* Fecha o socket e termina */
    close(sockfd);
    exit(0);
}