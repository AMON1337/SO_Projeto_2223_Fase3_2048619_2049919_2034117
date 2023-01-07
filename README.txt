NOTA:   ALTERAR OS PRINTS COM OS ACONTECIMENTOS, estão desatualizados!
NOTA2:  Discutir o menu final do simulador!
NOTA3:  Impedir clientes de ficarem á espera para entrar nas filas de espera!

Projeto SO 22/23 Simulação de uma discoteca!
Alunos: 
Filipe Lopes - 2048619
Tom Mendonça - 2049919
José Branco  - 2034117

Instruções:

1- Executar o comando make clean e depois o comando make.
2- Ligar o monitor.
3- Ligar o simulador.
4- Executar o Projeto.

Nota: O simulador só executa depois do monitor se ter ligado.


*--- Usamos os sockets sockets-unix-stream para o nosso projeto  ---*

unix-stream-server.c = monitor.c
util-stream-server.c = monitor_config.c

unix-stream-client.c = simulador.c
util-stream-client.c = simulador_config.c

*---                                                             ---*

Possíveis Problemas:
Efetuar -ggc version, caso der erro:

*Para conseguir efetuar compilações com gcc no Ubuntu é poderá necessário efetuar a instalação:
https://linuxize.com/post/how-to-install-gcc-compiler-on-ubuntu-18-04/

Ficheiro de configurações iniciais do Simulador (simulador_config_inicial.txt):
Limitações:
    Não alterar o número de zonas da discoteca.
    Não alterar o nome das zonas.
Pode fazer:
    Alterar a Capacidade de cada zona e da fila.
    Alterar as Probabilidades.

Acontecimentos do projeto:

60 : Abertura da Discoteca
69 : Encerramento da Discoteca

00 : Entrada - Discoteca
01 : Entrada - Pista de Dança
02 : Entrada - Zona VIP
03 : Entrada - WC
04 : Entrada - Restaurante

10 : Espera na fila - Discoteca
11 : Espera na fila - Pista de Dança
12 : Espera na fila - Zona VIP
13 : Espera na fila - WC

20 : Desistência da fila - Discoteca
21 : Desistência da fila - Pista de Dança
22 : Desistência da fila - Zona VIP
23 : Desistência da fila - WC

30 : Saída - Discoteca
31 : Saída - Pista de Dança
32 : Saída - Zona VIP
33 : Saída - WC
34 : Saída - Restaurante
38 : Saída - Expulso da Zona VIP
39 : Saída - Expulso da Discoteca
