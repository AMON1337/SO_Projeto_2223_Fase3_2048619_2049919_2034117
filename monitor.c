//  Monitor sockets-unix-stream dado nas Aulas TP de Sockets.
//  Servidor = Monitor
//  Cliente = Simulador
//  Monitor recebe dados do Cliente e mostra no ecrã

#include "unix.h" //Tem as bibliotecas comuns do projeto

main(void)
{
    int sockfd, newsockfd, clilen, childpid, servlen;
    struct sockaddr_un cli_addr, serv_addr;

    /* Cria socket stream */

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_dump("monitor: can't open stream socket");

    /*  Primeiro uma limpeza preventiva!
        Dados para o socket stream: tipo + nome do ficheiro.
        O ficheiro serve para que os clientes possam identificar o servidor */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);

    /* O servidor é quem cria o ficheiro que identifica o socket.
       Elimina o ficheiro, para o caso de algo ter ficado pendurado.
       Em seguida associa o socket ao ficheiro.
       A dimensão a indicar ao bind não é a da estrutura, pois depende
       do nome do ficheiro */

    servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    unlink(UNIXSTR_PATH);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
        err_dump("monitor, can't bind local address");

    /* Servidor pronto a aceitar 5 clientes para o socket stream */

    listen(sockfd, 5);

    // Texto extra ao iniciar - NÃO ESSENCIAL para o funcionamento dos sockets
    system("clear");                                    // limpar a consola
    printf("Monitor ONLINE\n");                         // Texto para dizer que o monitor está ligado
    printf("Esperando a ligação com o simulador...\n"); // Texto para dizer que o monitor está ligado
    // Texto extra ai iniciar - NÃO ESSENCIAL para o funcionamento dos sockets

    for (;;)
    {

        /* Não esquecer que quando o servidor aceita um cliente cria um
           socket para comunicar com ele. O primeiro socket (sockfd) fica
           � espera de mais clientes */

        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           &clilen);
        if (newsockfd < 0)
            err_dump("monitor: accept error");

        /* Lança processo filho para lidar com o cliente */

        if ((childpid = fork()) < 0)
            err_dump("monitor: fork error");

        else if (childpid == 0)
        {

            /* Processo filho que vai atender o cliente.
               Fechar sockfd é sanitário, já que não é
               utilizado pelo processo filho.
               Os dados recebidos do cliente s�o reenviados
               para o cliente */

            close(sockfd);
            str_echo(newsockfd);
            exit(0);
        }

        /* Processo pai.
           Fechar newsockfd é sanitário, já que não é
           utilizado pelo processo pai */

        close(newsockfd);
    }
}
