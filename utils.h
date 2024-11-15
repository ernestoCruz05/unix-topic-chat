
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
  char errorMenssagem[300];
  int pid;
} errorStruct;

typedef struct{
  char topico[20];
  int pid;
  int tipo;                               // 1- Desinscrever ; 0 - Inscrever
} pedidoStruct

typedef struct {
    int pid;
    char fifo_name[128];
    char nome[20];
    int numTopicos;
    topico topicosIns[MAX_CLIENTES];
} cliente;

typedef struct {
  char nomeTopico[20];
  int contInscritos;
  char fifo_name[128];
  int estado;                             // 1 - Bloqueado ; 0 - Desbloqueado
} topico;

#endif // !TP_SO_24_25_H


