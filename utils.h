
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
#define FED_FIFO "feedFIFO"

typedef struct{
  char menssagem[300];
  int pid;
  char nome[20];
  char topico[20];
} msgStruct;


#endif // !TP_SO_24_25_H


