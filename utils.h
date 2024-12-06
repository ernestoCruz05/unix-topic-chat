
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
#include <pthread.h>
#include <sys/select.h>


#define MAN_FIFO "managerFIFO"
#define MAX_CLIENTES 10
#define MAX_TOPICOS 20
#define MAX_MENSSAGENS 5

typedef struct{
  char menssagem[300];
  int pid;
  int duracao;
  char nome[20];
  char topico[20];

  //Flags
  int fechado;
  int bloqueado;
} msgStruct;

typedef struct{
  char menssagem[300];
  int pid;
  int tempoVida;
  char topico[20];
  char nome[20];
} msgPersStruct;


typedef struct {
  char errorMenssagem[300];
  int pid;
} errorStruct;

typedef struct{
  char topico[20];
  char username[20];
  int pid;
  char FIFO[128];
  int tipo;                               // 0 - Inscrever ; 1 - Desinscrever ; 2 - Validar nome ; 3 - Saida
} pedidoStruct;

typedef struct {
  int resposta;                           // 0 - Pedido não aceite ; 1 - Pedido aceite
} respostaStruct;


typedef struct {
  char nomeTopico[20];
  int contInscritos;
  int estado;                             // 1 - Bloqueado ; 0 - Desbloqueado
} topico;

typedef struct {
  int pid;
  char nome[20];
  int numTopicos;
  char FIFO[128];
  topico topicosIns[MAX_CLIENTES];
} cliente;

typedef struct {
  pthread_mutex_t *m;
  int stop;
} TDADOS;

#endif // !TP_SO_24_25_H


