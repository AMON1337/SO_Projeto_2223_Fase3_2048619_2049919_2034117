Projeto SO 22/23 Simulação de uma Discoteca!
Alunos: 
Filipe Lopes - 2048619
Tom Mendonça - 2049919
José Branco  - 2034117

Instruções para colocar o projeto a funcionar:

1- Executar o comando make clean e depois o comando make.
2- Ligar o monitor.
3- Ligar o simulador.
4- Executar o Projeto.

Nota: O simulador só executa depois do monitor se ter ligado.


**Usamos os sockets sockets-unix-stream, dados pelos professores, no nosso projeto**

Possível problema, utilizador de MAC!
Poderá ser necessário a instalação do gcc compiler.

*Para conseguir efetuar compilações com gcc no Ubuntu é poderá necessário efetuar a instalação:
https://linuxize.com/post/how-to-install-gcc-compiler-on-ubuntu-18-04/


Acontecimentos do Projeto

60 : Abertura da Discoteca
69 : Encerramento da Discoteca

00 : Entrada - Discoteca
01 : Entrada - Pista de Dança
02 : Entrada - Zona VIP
03 : Entrada - WC
04 : Entrada - Restaurante
05 : Entrada - Restaurante-Buffet

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
35 : Saída - Restaurante-Buffet
38 : Saída - Expulso da Zona VIP
39 : Saída - Expulso da Discoteca

41 : Fila cheia - Pista de Dança
42 : Fila cheia - Zona VIP
43 : Fila cheia - WC
44 : Fila cheia - Restaurante
