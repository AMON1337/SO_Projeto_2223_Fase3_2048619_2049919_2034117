 **NOTA** ALTEREI A CHANCE DE SER VIP de 15 para 20!!

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

O simulador só executa depois do monitor se ter ligado.


*--- Usamos os sockets sockets-unix-stream para o nosso projeto  ---*

unix-stream-server.c = monitor.c
util-stream-server.c = monitor_config.c

unix-stream-client.c = simulador.c
util-stream-client.c = simulador_config.c

*---                                                             ---*


Acontecimentos do projeto:

60 : Abertura da Discoteca
69 : Encerramento da Discoteca

00 : Entrada para a Discoteca
01 : Entrada pista de dança
02 : Entrada zona VIP
03 : Entrada na WC
04 : Entrada no restaurante

10 : Espera na fila - Discoteca
11 : Espera na fila - Pista de dança
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
34 : Saída - restaurante
