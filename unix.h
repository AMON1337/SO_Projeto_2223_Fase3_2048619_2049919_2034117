#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
// Bibliotecas úteis para o nosso Projeto
#include <stdbool.h> //Poder usar bools
#include <string.h>  //Poder usar srings

#define UNIXSTR_PATH "/tmp/s.SoProj2223"
#define UNIXDG_PATH "/tmp/s.SoProj2223"
#define UNIXDG_TMP "/tmp/dgSoProj2223"
