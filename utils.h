
#ifndef TP_SO_24_25_H
#define TP_SO_24_25_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define MAN_FIFO "managerFIFO"
#define MAX_CLIENTES 1
#define MAX_TOPICOS 20


typedef struct{
  char menssagem[300];
  int pid;
  int duracao;
  char nome[20];
  char topico[20];
} msgStruct;


typedef struct {
    int pid;
    char fifo_name[128];
} cliente;

typedef struct {
  char nomeTopico[20];
  int contInscritos;
  cliente clientes[MAX_CLIENTES];
  char fifo_name[128];
} topico;

#endif // !TP_SO_24_25_H


