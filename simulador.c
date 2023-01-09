#include "unix.h"      //Tem as bibliotecas comuns do projeto
#include <pthread.h>   //Usar tarefas para simular uma pessoa
#include <time.h>      //Tempo da simulação
#include <semaphore.h> //Usar Semáforos

// Comunicação com o Monitor //
#define MAXLINE 512 // Tamanho da Mensagem

/* Cria socket stream */
int sockfd, servlen;
struct sockaddr_un serv_addr;

// Ler mensagem do Monitor para iniciar/fechar simulação
int n;                // Número de caracteres
char buffer[MAXLINE]; // Iniciar/Fechar Simulação
int valor;            // Comparar buffer com string

// Sincronização das ZONAS //
sem_t sem_EnviarAcontMonitor; // Semáforo para o envio de uma mensagem para o monitor, só pode ser enviado uma mensagem de cada vez, iniciado a 1

// Discoteca (zona0, z0)
sem_t sem_FilaDiscoteca; // Semáforo da fila de espera da discoteca
sem_t sem_Discoteca;     // Semáforo das pessoas que estão dentro da discoteca
// Pista de Dança (zona1, z1)
sem_t sem_FilaPistaDanca; // Semáforo da fila de espera da pista de dança
sem_t sem_PistaDanca;     // Semáforo das pessoas que estão na pista de dança
// Zona VIP (zona2, z2)
sem_t sem_FilaZonaVIP; // Semáforo da fila de espera da zona vip
sem_t sem_ZonaVIP;     // Semáforo das pessoas que estão na zona vip
// WC (zona3, z3)
sem_t sem_FilaWC; // Semáforo da fila de espera da WC
sem_t sem_WC;     // Semáforo das pessoas que estão na WC
// Restaurante (zona4, z4)
sem_t sem_Restaurante;       // Semáforo das pessoas que estão no restaurante
sem_t sem_RestauranteBuffet; // Semáforo das pessoas que estão no buffet

// Mutex/Trincos para verificar se as filas das zonas estão cheias
/* Zona 0 (discoteca) não faz mal as pessoas esperarem para entrar na fila
   porque queremos que todos os clientes tentem entrar na discoteca*/
pthread_mutex_t mutex_nClientes_Fila_PistaDanca;
pthread_mutex_t mutex_nClientes_Fila_ZonaVIP;
pthread_mutex_t mutex_nClientes_Fila_WC;
pthread_mutex_t mutex_nClientes_Restaurante;
// Variáveis protegidas pelos trincos
int nClientes_Fila_PistaDanca = 0;
int nClientes_Fila_ZonaVIP = 0;
int nClientes_Fila_WC = 0;
int nClientes_Restaurante = 0;

// Estruturas de dados //
typedef struct cliente // tarefa = cliente
{
    int id_cliente;    // ID do cliente
    int acontecimento; // O que esta a fazer agora
    int tempo;         // Tempo em que ocorreu o acontecimento
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
    int prob_sair_disco;
};

struct discoteca disco; // Informação da configuração inicial da nossa DISCOTECA

//  Alterar o número de clientes tem de alterar todos estes valores igualmente, senão vai dar erro!
struct cliente thread_array[500]; // Serão criados 500 tarefas/clientes
pthread_t threads[500];           // Serão criados 500 tarefas/clientes
int capacidadeDisco = 500;        // Utilizado para criar tarefas no int main();

// Fechar Tarefas
int tarefasExit = 0;               // Número de tarefas que foram destruídas
pthread_mutex_t mutex_tarefasExit; // Trinco para a seccção crítica

// Tarefa para abrir/fechar a discoteca //
typedef struct horarioDiscoteca
{
    int abrirDiscoteca;  // Tempo para abrir a discoteca
    int fecharDiscoteca; // Tempo para fechar a discoteca
};

struct horarioDiscoteca horarioDiscoteca;
pthread_t abrirFecharDisco;

// Mutex para abrir/fechar discoteca;
pthread_mutex_t mutex_afDiscoteca;

int discotecaAberta = 0;  // 0 - Disco Fechada, 1 - disco Aberta, 2 - Disco Encerrar
int tempoDiscoAbre = 5;   // A Disco abre 5s depois da simulação começar
int tempoDiscoFecha = 65; // A Disco fecha 65s depois da simulação começar
//  Nota: 65-5 = 60s que discoteca fica aberta ao público

// Tempo - usamos como referencia para os acontecimentos da simulação
struct timespec ts;

// Lê a configuração inicial de um ficheiro .txt
void lerConfigInicial()
{
    char nome[MAXLINE];
    char valor[MAXLINE];

    printf("**Loading das configurações iniciais**\n");

    FILE *fpConfig = fopen("simulador_config_inicial.txt", "r"); // Abrir para ler o ficheiro de configuração incial da discoteca

    if (fpConfig == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro com a configuração inicial!\nImpossível continuar!\n");

        return;
    }

    // Obter as variáveis
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
        if (strcmp(nome, "prob_sair_disco") == 0)
            disco.prob_sair_disco = atoi(valor);
    }
    printf("**Loading Completo**\n\n");
    fclose(fpConfig);
}

// Escreve no ficheiro de LOGS da simulação logSimulador.log
void logInicialDisco()
{
    FILE *fpLog = fopen("logSimulador.log", "w"); // Abrir e/ou Criar o ficheiro logs para escrever

    if (fpLog == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro de logs!\nImpossível continuar!\n");
        return;
    }
    else
    {
        fprintf(fpLog, "!------------ LOGS DA SIMULAÇÃO ------------!\n\n"); // escreve no ficheiro de logs do simulador
        fprintf(fpLog, "*--Configuração Inicial da Discoteca--*\n");
    }

    // Escreves nos logs do simulador os dados internos inciais da simulação
    fprintf(fpLog, "Numero de zonas na discoteca: %d\n", disco.n_zonas);
    fprintf(fpLog, "Nome da zona 0: %s\n", disco.z0_nome);
    fprintf(fpLog, "Nome da zona 1: %s\n", disco.z1_nome);
    fprintf(fpLog, "Nome da zona 2: %s\n", disco.z2_nome);
    fprintf(fpLog, "Nome da zona 3: %s\n", disco.z3_nome);
    fprintf(fpLog, "Nome da zona 4: %s\n", disco.z4_nome);

    fprintf(fpLog, "\nCapacidades máximas das zonas\n");
    fprintf(fpLog, "Capacidade máxima da zona 0: %d\n", disco.z0_max);
    fprintf(fpLog, "Capacidade máxima da zona 1: %d\n", disco.z1_max);
    fprintf(fpLog, "Capacidade máxima da zona 2: %d\n", disco.z2_max);
    fprintf(fpLog, "Capacidade máxima da zona 3: %d\n", disco.z3_max);
    fprintf(fpLog, "Capacidade máxima da zona 4: %d\n", disco.z4_max);

    fprintf(fpLog, "\nCapacidades máximas das filas das zonas\n");
    fprintf(fpLog, "Capacidade máxima da fila da zona 0: %d\n", disco.z0_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 1: %d\n", disco.z1_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 2: %d\n", disco.z2_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 3: %d\n", disco.z3_fila_max);
    fprintf(fpLog, "Capacidade máxima da fila da zona 4: %d\n", disco.z4_fila_max);

    fprintf(fpLog, "\nProbabilidades da simulação\n");
    fprintf(fpLog, "Probabilidade de desistir da fila: %d\n", disco.prob_desistir_fila);
    fprintf(fpLog, "Probabilidade de ser VIP: %d\n", disco.prob_ser_vip);
    fprintf(fpLog, "Probabilidade de ser expluso: %d\n", disco.prob_ser_expulso);
    fprintf(fpLog, "Probabilidade de sair da Discoteca: %d\n", disco.prob_sair_disco);

    // Acontecimentos da simulação
    fprintf(fpLog, "\n*--Acontecimentos da simulação--*\n"
                   "60 : Abertura da Discoteca\n"
                   "69 : Encerramento da Discoteca\n\n"
                   "00 : Entrada - Discoteca\n"
                   "01 : Entrada - Pista de Dança\n"
                   "02 : Entrada - Zona VIP\n"
                   "03 : Entrada - WC\n"
                   "04 : Entrada - Restaurante\n"
                   "05 : Entrada - Restaurante-Buffet\n\n"
                   "10 : Espera na fila - Discoteca\n"
                   "11 : Espera na fila - Pista de Dança\n"
                   "12 : Espera na fila - Zona VIP\n"
                   "13 : Espera na fila - WC\n\n"
                   "20 : Desistência da fila - Discoteca\n"
                   "21 : Desistência da fila - Pista de Dança\n"
                   "22 : Desistência da fila - Zona VIP\n"
                   "23 : Desistência da fila - WC\n\n"
                   "30 : Saída - Discoteca\n"
                   "31 : Saída - Pista de Dança\n"
                   "32 : Saída - Zona VIP\n"
                   "33 : Saída - WC\n"
                   "34 : Saída - Restaurante\n"
                   "35 : Saída - Restaurante-Buffet\n"
                   "38 : Saída - Expulso da Zona VIP\n"
                   "39 : Saída - Expulso da Discoteca\n\n"
                   "41 : Fila cheia - Pista de Dança\n"
                   "42 : Fila cheia - Zona VIP\n"
                   "43 : Fila cheia - WC\n"
                   "44 : Fila cheia - Restaurante\n\n");

    // Preparar para receber os Acontecimentos da simulação:
    fprintf(fpLog, "\n\n* <---> ACONTECIMENTOS DA SIMULAÇÃO <---> *\n\n");

    fclose(fpLog);
}

// Faz um print do estado inicial da discoteca no ecrã da simulação
void printInicialDisco()
{
    printf("*--Configuração Inicial da Discoteca--*\n");

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

    printf("\nProbabilidades da simulação\n"); // Probabilidades
    printf("Probabilidade de desistir da fila: %d\n", disco.prob_desistir_fila);
    printf("Probabilidade de ser VIP: %d\n", disco.prob_ser_vip);
    printf("Probabilidade de ser expluso: %d\n", disco.prob_ser_expulso);
    printf("Probabilidade de sair da Discoteca: %d\n", disco.prob_sair_disco);

    // Acontecimentos da simulação
    printf("\n*--Acontecimentos da simulação--*\n"
           "60 : Abertura da Discoteca\n"
           "69 : Encerramento da Discoteca\n\n"
           "00 : Entrada - Discoteca\n"
           "01 : Entrada - Pista de Dança\n"
           "02 : Entrada - Zona VIP\n"
           "03 : Entrada - WC\n"
           "04 : Entrada - Restaurante\n"
           "05 : Entrada - Restaurante-Buffet\n\n"
           "10 : Espera na fila - Discoteca\n"
           "11 : Espera na fila - Pista de Dança\n"
           "12 : Espera na fila - Zona VIP\n"
           "13 : Espera na fila - WC\n\n"
           "20 : Desistência da fila - Discoteca\n"
           "21 : Desistência da fila - Pista de Dança\n"
           "22 : Desistência da fila - Zona VIP\n"
           "23 : Desistência da fila - WC\n\n"
           "30 : Saída - Discoteca\n"
           "31 : Saída - Pista de Dança\n"
           "32 : Saída - Zona VIP\n"
           "33 : Saída - WC\n"
           "34 : Saída - Restaurante\n"
           "35 : Saída - Restaurante-Buffet\n"
           "38 : Saída - Expulso da Zona VIP\n"
           "39 : Saída - Expulso da Discoteca\n\n"
           "41 : Fila cheia - Pista de Dança\n"
           "42 : Fila cheia - Zona VIP\n"
           "43 : Fila cheia - WC\n"
           "44 : Fila cheia - Restaurante\n\n");
}

// Envia o acontecimento para o Monitor/ Escreva na logSimulador.log e print no ecrã do simulador
void enviarAcontecimento(int id_cliente, int acontecimento, int tempo, int vip)
{

    sem_wait(&sem_EnviarAcontMonitor); // Semáforo de exlusão mútua, só pode ser enviado um acontecimento de cada vez
    // Envia Acontecimento para o Monitor
    bzero(buffer, MAXLINE);                                                  // limpa o buffer
    sprintf(buffer, "%d.%d.%d.%d\n", id_cliente, acontecimento, tempo, vip); // coloca mensagem no buffer

    str_cli(buffer, sockfd); // Envia a mensagem (acontecimento) para o monitor!

    // Escreve o Acontecimento no logSimulador.log
    FILE *fpLog = fopen("logSimulador.log", "a+"); // Abrir o ficheiro logs para concatenar o acontecimento
    if (fpLog == NULL)
    {
        printf("Ocorreu um problema ao abrir o ficheiro de logs!\nImpossível continuar!\n");
        return;
    }
    fprintf(fpLog, "Cliente ID: %d, Acontecimento: %d, Tempo: %d, VIP: %d\n", id_cliente, acontecimento, tempo, vip);
    fclose(fpLog);

    // Print do acontecimento no ecrã no simulador
    printf("Cliente ID: %d, Acontecimento: %d, Tempo: %d, VIP: %d\n", id_cliente, acontecimento, tempo, vip);

    sem_post(&sem_EnviarAcontMonitor); // Assinala o Semáforo para a próxima tarefa
}

// Rotina do Cliente depois de entrar na discoteca
void RotinaClienteDiscoteca(struct cliente *cliente)
{
    // Cliente decide em que zona deseja entrar!
    usleep(1000000 + rand() % 5000000); // Tempo de decisão 1s + 0-5s
    int escolha;
    int modificador;
    // Escolha de zona: 1 - Pista de Dança, 2 - Zona VIP, 3 - WC ou 4 - Restaurante
    escolha = rand() % 4 + 1; // Escolher número random de 1 a 4

    // Modificador de escolha: VIP-> zona VIP, não VIP -> pista de Dança
    /* Modificador existe porque é provável que em e VIP entra na Disco e vá para a zona VIP
    e quem não é VIP é provável que vá para a atracão principal a Pista de Dança*/
    if (cliente->vip == 0) // Não é VIP
    {
        modificador = rand() % 3; // 66% de chance de escolher 0 ou 1, 33% de escolher 2
        if (modificador != 2)     // saiu 0 ou 1
            escolha = 1;          // Cliente  não VIP muda a escolha de zona para a Pista de Dança
    }
    else if (cliente->vip == 1) // É VIP
    {
        modificador = rand() % 2; // 50% de chance de escolher 0 ou 1
        if (modificador == 0)     // saiu 0
            escolha = 2;          // Cliente VIP muda a escolha de zona para a zona VIP
    }

    switch (escolha)
    {
    case 1: // Pista de Dança

        // Verifica se tem espaço na fila
        // Secção crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_PistaDanca);
        if (nClientes_Fila_PistaDanca == disco.z1_fila_max) // Caso a fila esteja cheia
        {
            pthread_mutex_unlock(&mutex_nClientes_Fila_PistaDanca); // Desbloqueia o trinco

            // Notificar Acontecimento
            cliente->acontecimento = 41; // Acontecimento: Fila cheia - Pista de Dança
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

            RotinaClienteDiscoteca(cliente); // chama função, ou seja, "O cliente volta a escolher onde quer ir!"
        }
        pthread_mutex_unlock(&mutex_nClientes_Fila_PistaDanca);

        // Entrar na Fila de Espera da Pista de Dança
        sem_wait(&sem_FilaPistaDanca); // Entra na fila de espera da pista de dança

        // Atualizar varíavel nClientes na fila da Pista de Dança
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_PistaDanca);
        nClientes_Fila_PistaDanca++; // +1 cliente na fila
        pthread_mutex_unlock(&mutex_nClientes_Fila_PistaDanca);

        // Notificar Acontecimento
        cliente->acontecimento = 11; // Acontecimento: Espera na fila - Pista de Dança
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente quer desistir da fila?
        usleep(1000000 + rand() % 1000000); // Tempo de decisão 1s + 0-1s
        if ((rand() % 100 + 1) <= disco.prob_desistir_fila)
        {
            // Desistiu da fila!
            sem_post(&sem_FilaPistaDanca); // Sai da Fila de espera da pista de dança

            // Atualizar varíavel nClientes na fila da Pista de Dança
            // Secção Crítica
            pthread_mutex_lock(&mutex_nClientes_Fila_PistaDanca);
            nClientes_Fila_PistaDanca--; // -1 cliente na fila
            pthread_mutex_unlock(&mutex_nClientes_Fila_PistaDanca);

            // Notificar Acontecimento
            cliente->acontecimento = 21; // Acontecimento: Desistência da fila - Pista de Dança
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
            break;
        }

        // Cliente quer continuar na fila!
        sem_wait(&sem_PistaDanca);     // Entra na pista de dança
        sem_post(&sem_FilaPistaDanca); // Saída da Fila de espera da pista de dança

        // Atualizar varíavel nClientes na fila da Pista de Dança
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_PistaDanca);
        nClientes_Fila_PistaDanca--; // -1 cliente na fila de espera
        pthread_mutex_unlock(&mutex_nClientes_Fila_PistaDanca);

        // Notificar Acontecimento
        cliente->acontecimento = 01; // Acontecimento: Entrada - Pista de Dança
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente diverte-se na pista de dança...
        usleep(5000000 + rand() % 10000000); // Tempo de  diversão 5 + 0-10s

        // Cliente cansado de Dançar, quer sair da Pista de Dança!
        sem_post(&sem_PistaDanca); // Saída da Pista de Dança
        // Notificar Acontecimento
        cliente->acontecimento = 31; // Acontecimento: Saída - Pista de Dança
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
        break;

    case 2: // Zona VIP

        // Verifica se o Cliente é VIP
        if (cliente->vip == 0) // Cliente não é VIP, não pode entrar na zona vip
        {
            // Tentou entrar na zona vip sem ser vip é expulso da zona
            // Notificar Acontecimento
            cliente->acontecimento = 38; // Acontecimento: Saída - Expulso da Zona VIP
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
            break;
        }

        // Verifica se tem espaço na fila
        // Secção crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_ZonaVIP);
        if (nClientes_Fila_ZonaVIP == disco.z2_fila_max) // Caso a fila esteja cheia
        {
            pthread_mutex_unlock(&mutex_nClientes_Fila_ZonaVIP); // Desbloqueia o trinco

            // Notificar Acontecimento
            cliente->acontecimento = 42; // Acontecimento: Fila cheia - Zona VIP
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

            RotinaClienteDiscoteca(cliente); // chama função, ou seja, "O cliente volta a escolher onde quer ir!"
        }
        pthread_mutex_unlock(&mutex_nClientes_Fila_ZonaVIP);

        // Entrar na Fila de Espera da Zona VIP
        sem_wait(&sem_FilaZonaVIP); // Entra na fila de espera da zona vip

        // Atualizar varíavel nClientes na fila da zona VIP
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_ZonaVIP);
        nClientes_Fila_ZonaVIP++; // +1 cliente na fila
        pthread_mutex_unlock(&mutex_nClientes_Fila_ZonaVIP);

        // Notificar Acontecimento
        cliente->acontecimento = 12; // Acontecimento: Espera na fila - Zona VIP
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente quer desistir da fila de espera?
        usleep(1000000 + rand() % 2000000); // Tempo de decisão 1s + 0-2s
        if ((rand() % 100 + 1) <= disco.prob_desistir_fila)
        {
            // Desistiu da fila!
            sem_post(&sem_FilaZonaVIP); // Sai da Fila de espera da zona vip

            // Atualizar varíavel nClientes na fila da zona VIP
            // Secção Crítica
            pthread_mutex_lock(&mutex_nClientes_Fila_ZonaVIP);
            nClientes_Fila_ZonaVIP--; // -1 cliente na fila
            pthread_mutex_unlock(&mutex_nClientes_Fila_ZonaVIP);

            // Notificar Acontecimento
            cliente->acontecimento = 22; // Acontecimento: Desistência da fila - Zona VIP
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
            break;
        }

        // Cliente quer continuar na fila!
        sem_wait(&sem_ZonaVIP);     // Entra na zona VIP
        sem_post(&sem_FilaZonaVIP); // Saída da Fila de espera da zona vip

        // Atualizar varíavel nClientes na fila da zona VIP
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_ZonaVIP);
        nClientes_Fila_ZonaVIP--; // -1 cliente na fila
        pthread_mutex_unlock(&mutex_nClientes_Fila_ZonaVIP);

        // Notificar Acontecimento
        cliente->acontecimento = 02; // Acontecimento: Entrada - Zona VIP
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente diverte-se na zona VIP
        usleep(5000000 + rand() % 10000000); // Tempo de  diversão 5 + 0-10s

        // Cliente cansado de conviver com VIPs...
        sem_post(&sem_ZonaVIP); // Saída da Zona VIP
        // Notificar Acontecimento
        cliente->acontecimento = 32; // Acontecimento: Saída - Zona VIP
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
        break;

    case 3: // WC

        // Verifica se tem espaço na fila
        // Secção crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_WC);
        if (nClientes_Fila_WC == disco.z3_fila_max) // Caso a fila esteja cheia
        {
            pthread_mutex_unlock(&mutex_nClientes_Fila_WC); // Desbloqueia o trinco

            // Notificar Acontecimento
            cliente->acontecimento = 43; // Acontecimento: Fila cheia - WC
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

            RotinaClienteDiscoteca(cliente); // chama função, ou seja, "O cliente volta a escolher onde quer ir!"
        }
        pthread_mutex_unlock(&mutex_nClientes_Fila_WC);

        // Entrar na Fila de Espera da WC
        sem_wait(&sem_FilaWC); // Entra na fila de espera da WC

        // Atualizar varíavel nClientes na fila da WC
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_WC);
        nClientes_Fila_WC++; // +1 cliente na fila
        pthread_mutex_unlock(&mutex_nClientes_Fila_WC);

        // Notificar Acontecimento
        cliente->acontecimento = 13; // Acontecimento: Espera na fila - WC
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente quer desistir da fila de espera?
        usleep(1000000); // Tempo de decisão 1s
        if ((rand() % 100 + 1) <= disco.prob_desistir_fila)
        {
            // Desistiu da fila de espera!
            sem_post(&sem_FilaWC); // Sai da Fila de espera da WC

            // Atualizar varíavel nClientes na fila da WC
            // Secção Crítica
            pthread_mutex_lock(&mutex_nClientes_Fila_WC);
            nClientes_Fila_WC--; // -1 cliente na fila
            pthread_mutex_unlock(&mutex_nClientes_Fila_WC);

            // Notificar Acontecimento
            cliente->acontecimento = 23; // Acontecimento: Desistência da fila - WC
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
            break;
        }

        // Cliente quer continuar na fila!
        sem_wait(&sem_WC);     // Entra na WC
        sem_post(&sem_FilaWC); // Saída da fila de espera da WC

        // Atualizar varíavel nClientes na fila da WC
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Fila_WC);
        nClientes_Fila_WC--; // -1 cliente na fila
        pthread_mutex_unlock(&mutex_nClientes_Fila_WC);

        // Notificar Acontecimento
        cliente->acontecimento = 03; // Acontecimento: Entrada - WC
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente faz o que precisa na WC
        usleep(500000 + rand() % 2000000); // Tempo necessário 0,5s + 0-2s
        // Cliente acabou de usar a WC
        sem_post(&sem_WC); // Saída da WC
        // Notificar Acontecimento
        cliente->acontecimento = 33; // Acontecimento: Saída - WC
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
        break;
    case 4: // Restaurante;

        // Verifica se tem espaço na fila
        // Secção crítica
        pthread_mutex_lock(&mutex_nClientes_Restaurante);
        if (nClientes_Restaurante == disco.z4_max) // Caso o restaurante esteja cheio
        {
            pthread_mutex_unlock(&mutex_nClientes_Restaurante); // Desbloqueia o trinco

            // Notificar Acontecimento
            cliente->acontecimento = 44; // Acontecimento: Fila cheia - Restaurante
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

            RotinaClienteDiscoteca(cliente); // chama função, ou seja, "O cliente volta a escolher onde quer ir!"
        }
        pthread_mutex_unlock(&mutex_nClientes_Restaurante);

        // Entrada no Restaurante
        sem_wait(&sem_Restaurante);

        // Atualizar varíavel nClientes no Restaurante
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Restaurante);
        nClientes_Restaurante++; // +1 cliente no restaurante
        pthread_mutex_unlock(&mutex_nClientes_Restaurante);

        // Notificar Acontecimento
        cliente->acontecimento = 04; // Acontecimento: Entrada - Restaurante
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Vai buscar comida ao Buffet
        sem_wait(&sem_RestauranteBuffet);

        // Notificar Acontecimento
        cliente->acontecimento = 05; // Acontecimento: Entrada - Restaurante-Buffet
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        usleep(1000000 + rand() % 1000000); // tempo a ir buscar o buffet 1s + 0-1s

        // Já tem a comida e volta para a sua mesa
        sem_post(&sem_RestauranteBuffet);

        // Notificar Acontecimento
        cliente->acontecimento = 35; // Acontecimento: Saída - Restaurante-Buffet
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Cliente volta para a sua mesa e come a sua refeição
        usleep(1000000 + rand() % 2000000); // Tempo necessário 1s + 0-2s

        // Cliente acabou de comer a sua refeição
        sem_post(&sem_Restaurante); // Cliente sai do Restaurante

        // Atualizar varíavel nClientes no Restaurante
        // Secção Crítica
        pthread_mutex_lock(&mutex_nClientes_Restaurante);
        nClientes_Restaurante--; // -1 cliente no restaurante
        pthread_mutex_unlock(&mutex_nClientes_Restaurante);

        // Notificar Acontecimento
        cliente->acontecimento = 34; // Acontecimento: Saída - Restaurante
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);
        break;
    default: // Escolha inválida //ERRO
        printf("O cliente %d, realizou uma escolha inválida!\n", cliente->id_cliente);
        break;
    }

    // Cliente andou à porrada com os seguranças?
    if ((rand() % 100 + 1) <= disco.prob_ser_expulso)
    {
        sem_post(&sem_Discoteca); // Sai da Discoteca (à força)
        // Notificar acontecimento
        cliente->acontecimento = 39; // Acontecimento: Saída - Expulso
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Secção Crítica
        pthread_mutex_lock(&mutex_tarefasExit);
        tarefasExit++; // adiciona tarefa fechada
        pthread_mutex_unlock(&mutex_tarefasExit);
        // Fecha a tarefa
        pthread_exit(cliente);
    }

    // Cliente quer sair da Discoteca?
    if ((rand() % 100 + 1) <= disco.prob_sair_disco)
    {
        sem_post(&sem_Discoteca); // Sai da Discoteca (por decisão sua)
        // Notificar Acontecimento
        cliente->acontecimento = 30; // Acontecimento: Saída - Discoteca
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Secção Crítica
        pthread_mutex_lock(&mutex_tarefasExit);
        tarefasExit++; // adiciona tarefa fechada
        pthread_mutex_unlock(&mutex_tarefasExit);
        // Fecha a tarefa
        pthread_exit(cliente);
    }

    // A Discoteca está encerrada?
    // Secção Crítica
    pthread_mutex_lock(&mutex_afDiscoteca);
    if (discotecaAberta == 0 || discotecaAberta == 2) // Se discoteca fechada 0-fechada, 2-fechada, mas abriu hoje
    {
        pthread_mutex_unlock(&mutex_afDiscoteca); // Liberta o trinco

        sem_post(&sem_Discoteca);    // Sai da Discoteca (porque está fechada!)
        cliente->acontecimento = 30; // Acontecimento: Saída - Discoteca
        timespec_get(&ts, TIME_UTC); // Tempo atual
        cliente->tempo = ts.tv_sec;
        enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

        // Secção Crítica
        pthread_mutex_lock(&mutex_tarefasExit);
        tarefasExit++; // adiciona tarefa fechada
        pthread_mutex_unlock(&mutex_tarefasExit);
        // Fecha a tarefa
        pthread_exit(cliente);
    }
    pthread_mutex_unlock(&mutex_afDiscoteca);

    // Cliente decidiu ficar na discoteca para apreciar outras atividades
    RotinaClienteDiscoteca(cliente); // Chama a rotina outra vez e começa de novo!
}

// Rotina do Cliente na Discoteca,
void *rotinaCliente(void *ptr)
{
    // Identificar o cliente;
    struct cliente *cliente;
    cliente = (struct cliente *)ptr;

    // Probabilidade de ser VIP
    if (rand() % 100 + 1 <= disco.prob_ser_vip) // random de 1-100 for <= prob_ser_vip, então é vip
        cliente->vip = 1;                       // YES VIP
    else
        cliente->vip = 0; // NO VIP

    // Cliente entra na fila para a discoteca
    sem_wait(&sem_FilaDiscoteca); // Entra na fila de espera
    // Notificar Acontecimento
    cliente->acontecimento = 10; // Acontecimento: Espera na fila - Discoteca
    timespec_get(&ts, TIME_UTC); // Pega no tempo atual e guarda no cliente
    cliente->tempo = ts.tv_sec;
    enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

    // Espera na fila da discoteca para entrar
    usleep(1000000 + rand() % 3000000); // Espera1s + 0-3s

    // Enquanto a discoteca estiver fechada, espera que ela abra
    do
    {
        // Secção crítica;
        pthread_mutex_lock(&mutex_afDiscoteca);
        if (discotecaAberta == 1) // Se discoteca estiver aberta
        {
            pthread_mutex_unlock(&mutex_afDiscoteca); // Liberta o trinco
            break;                                    // Quebra o ciclo
        }
        else if (discotecaAberta == 2) // Discoteca abriu hoje, mas já fechou
        {
            pthread_mutex_unlock(&mutex_afDiscoteca); // Liberta o trinco

            usleep(rand() % 1000000);     // Espera 0-1s
            sem_post(&sem_FilaDiscoteca); // sai da fila de entrada da discoteca
            // Notificar Acontecimento
            cliente->acontecimento = 20; // Acontecimento: Desistência da fila - Discoteca
            timespec_get(&ts, TIME_UTC); // Tempo atual
            cliente->tempo = ts.tv_sec;
            enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

            // Secção Crítica
            pthread_mutex_lock(&mutex_tarefasExit);
            tarefasExit++; // adiciona tarefa fechada
            pthread_mutex_unlock(&mutex_tarefasExit);
            // Fecha a tarefa
            pthread_exit(cliente);
        }
        pthread_mutex_unlock(&mutex_afDiscoteca);
    } while (1);

    // Cliente quer entrar na Discoteca
    sem_wait(&sem_Discoteca);     // Tenta entrar na discoteca.
    sem_post(&sem_FilaDiscoteca); // Sai da fila de entrada
    // Notificar Acontecimento
    cliente->acontecimento = 00; // Acontecimento: Entrada - Discoteca
    timespec_get(&ts, TIME_UTC); // Pega no tempo atual e guarda no cliente
    cliente->tempo = ts.tv_sec;
    enviarAcontecimento(cliente->id_cliente, cliente->acontecimento, cliente->tempo, cliente->vip);

    // Função que trata da rotina do cliente dentro da discoteca
    RotinaClienteDiscoteca(cliente);

    /* Só por redundância / precausão */
    // Secção Crítica
    pthread_mutex_lock(&mutex_tarefasExit);
    tarefasExit++; // adiciona tarefa fechada
    pthread_mutex_unlock(&mutex_tarefasExit);
    // Fecha a tarefa
    pthread_exit(cliente);
}

void *RotinaAbrirDiscoteca(void *ptr)
{
    // Identificar horarioDiscoteca
    struct horarioDiscoteca *horarioDiscoteca;
    horarioDiscoteca = (struct horarioDiscoteca *)ptr;

    printf("Discoteca esta fechada ao iniciar a simulação!\nNinguém entra até o Manager abrir a Discoteca!\n");

    // Defenir tempo para abrir e fechar a discoteca
    timespec_get(&ts, TIME_UTC);
    horarioDiscoteca->abrirDiscoteca = ts.tv_sec + tempoDiscoAbre;   // Abrir a Discoteca em tempoDiscoAbre apartir de agora
    horarioDiscoteca->fecharDiscoteca = ts.tv_sec + tempoDiscoFecha; // Fechar a Discoteca em tempoDiscoFecha segundos apartir de agora

    // Verifica se esta no tempo de abrir a Discoteca
    while (ts.tv_sec < horarioDiscoteca->abrirDiscoteca)
    {
        timespec_get(&ts, TIME_UTC);
    }

    // Secção Crítica
    pthread_mutex_lock(&mutex_afDiscoteca);
    discotecaAberta = 1; // abre a discoteca
    pthread_mutex_unlock(&mutex_afDiscoteca);

    enviarAcontecimento(0, 60, ts.tv_sec, 1); // Envia o acontecimento de abrir a discoteca, id_cliente = 0 é o manager
    printf("Discoteca abriu as portas ao público! :)\n");

    // Verifica se esta no tempo de fechar a Discoteca
    while (ts.tv_sec < horarioDiscoteca->fecharDiscoteca)
    {
        timespec_get(&ts, TIME_UTC);
    }

    // Secção Crítica
    pthread_mutex_lock(&mutex_afDiscoteca);
    discotecaAberta = 2; // fecha discoteca pelo dia
    pthread_mutex_unlock(&mutex_afDiscoteca);

    enviarAcontecimento(0, 69, ts.tv_sec, 1); // Envia o acontecimento de abrir a discoteca, id_cliente = 0 é o manager
    printf("Discoteca fechou as portas ao público! :(\n");

    /* Fecha a tarefa +1 */
    // Secção Crítica
    pthread_mutex_lock(&mutex_tarefasExit);
    tarefasExit++; // adiciona tarefa fechada
    pthread_mutex_unlock(&mutex_tarefasExit);
    // Fecha a tarefa
    pthread_exit(horarioDiscoteca); // Fecha a tarefa
}

int main(void)
{
    // Váriaveis necessárias para a criação do socket no inicio //
    /* Cria socket stream */

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_dump("simulador: can't open stream socket");

    /* Primeiro uma limpeza preventiva!
       Dados para o socket stream: tipo + nome do ficheiro.
         O ficheiro identifica o monitor */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

    /* Tenta estabelecer uma ligação. Só funciona se o monitor tiver
         sido lançado primeiro (o monitor tem de criar o ficheiro e associar
         o socket ao ficheiro) */

    if (connect(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
        err_dump("O Simulador não consegue ligar-se ao Monitor!\n");

    /*______________________AQUI COMEÇA O NOSSO TRABALHO______________________*/
    system("clear"); // Limpar consola
    printf("Simulador ONLINE\n\n");

    // Let's make random, Random again :)
    srand(time(0));

    // Configuração Inicial
    lerConfigInicial(); // Lê do ficheiro incial a configuração inicial

    logInicialDisco();   // LOGS - Escreve nos logs do simulador o estado incial da discoteca
    printInicialDisco(); // ECRÃ da SIMULAÇÃO - Print do estado incial da discoteca

    // Esperar iniciar simulação, ou seja, espera receber mensagem do monitor para começar
    printf("\n\nEsperando para começar a simulação...\n");

    bzero(buffer, MAXLINE);            // Limpar Buffer
    n = read(sockfd, buffer, MAXLINE); // Esperar Mensagem do Monitor
    if (n < 0)
    {
        perror("Erro ao ler mensagem recebida do Monitor\n");
        exit(1);
    }

    // Compara o buffer com INICIAR
    valor = strcmp(buffer, "INICIAR");
    if (valor == 0) // Buffer é igual a INICIAR
    {
        printf("Iniciando a Simulação em 3 segundos...\n");
        usleep(3000000); // generates 3 second delay
    }
    else // Buffer é igual a FECHAR
    {
        printf("Cancelando Simulação!\n");
        close(sockfd);
        exit(0);
    }

    /// INICIAR SIMULAÇÃO ///

    // ---> Inicializar os Trincos <--- //
    pthread_mutex_init(&mutex_afDiscoteca, NULL); // Trinco para abrir ou fechar a discoteca
    pthread_mutex_init(&mutex_tarefasExit, NULL); // Trinco para seccção crítica da variável tarefasExit

    // Trincos das variáveis de controlo das filas das zonas
    pthread_mutex_init(&mutex_nClientes_Fila_PistaDanca, NULL); // Zona1
    pthread_mutex_init(&mutex_nClientes_Fila_ZonaVIP, NULL);    // Zona2
    pthread_mutex_init(&mutex_nClientes_Fila_WC, NULL);         // Zona3
    pthread_mutex_init(&mutex_nClientes_Restaurante, NULL);     // Zona4

    // ---> Inicializar os Semáforos <--- // Segundo argumento sem_init a 0, significa que é partilhado entre tarefas

    sem_init(&sem_EnviarAcontMonitor, 0, 1); // Iniciado a 1, exclusão mútua, só uma tarefa é que envia de cada vez o acontecimento ao monitor

    // Discoteca - Z0 (Zona 0)
    sem_init(&sem_FilaDiscoteca, 0, disco.z0_fila_max); // Semáforo para a fila de espera da discoteca
    sem_init(&sem_Discoteca, 0, disco.z0_max);          // Semáforo para a capacidade máxima da discoteca

    // Pista de Dança - Z1 (Zona 1)
    sem_init(&sem_FilaPistaDanca, 0, disco.z1_fila_max); // Semáforo para a fila de espera da pista de dança
    sem_init(&sem_PistaDanca, 0, disco.z1_max);          // Semáforo para a capacidade máxima da pista de dança

    // Zona Vip - Z2 (Zona 2)
    sem_init(&sem_FilaZonaVIP, 0, disco.z2_fila_max); // Semáforo para a fila de espera da zona vip
    sem_init(&sem_ZonaVIP, 0, disco.z2_max);          // Semáforo para a capacidade máxima da zona vip

    // WC - Z3 (Zona 3)
    sem_init(&sem_FilaWC, 0, disco.z3_fila_max); // Semáforo para a fila de espera da WC
    sem_init(&sem_WC, 0, disco.z3_max);          /*Semáforo para a cap max da WC,
             configuração inicial original 1, ou seja, Semáforo inicializado em exlusão mútua*/

    // Restaurante - Z4 (Zona 4)
    sem_init(&sem_Restaurante, 0, disco.z4_max);            // Semáforo para a capacidade máxima do restaurante
    sem_init(&sem_RestauranteBuffet, 0, disco.z4_fila_max); // Semáforo para a capacidade máxima do buffet do restaurante

    // Criar Tarefa para Abrir/Fechar a Discoteca
    if (pthread_create(&(abrirFecharDisco), NULL, *RotinaAbrirDiscoteca, &horarioDiscoteca) != 0)
    {
        printf("Erro ao criar a tarefa que controla a abertura e fecho da Discoteca");
        return 0;
    }

    // Criação das Tarefas (clientes)
    int i = 0;                            // Número de tarefas criadas
    for (i = 0; i < capacidadeDisco; i++) // Criar tarefas até capacidadeDisco (500) - Configuração original
    {
        usleep(125000); // Espera 0,125s
        if (pthread_create(&(threads[i]), NULL, *rotinaCliente, &thread_array[i]) != 0)
        {
            printf("Erro ao criar o cliente com o id:%d", i);
            return 0;
        }
        thread_array[i].id_cliente = i + 1; // i=0, id_cliente=1
    }

    // Enquanto as tarefas fechadas não forem igula as tarefas criadas
    while (tarefasExit != capacidadeDisco + 1) // tarefas clientes + tarefa que abre e fecha a disco
    {
    }
    printf("Todas as tarefas do Simulador fechadas com sucesso!\n");
    printf("Fechando o Simulador em 3 segundos...\n");
    usleep(3000000); // Espera 3 segundos

    /* Fecha o socket e termina */
    close(sockfd);
    exit(0);
}
