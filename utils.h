
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

/*
 *  Comentarios:
 *
 *  No manager tou a converter as menssagens normais para persistentes (se a duração for maior q 0) para adiciona-las a lista
 *  de menssagens persistentes mas depois quando as mando de volta para os clientes converto outra vez para normais, acho q isto
 *  esta a criar o problema de as persistentes serem mandadas duas vezes, mas n faço a minima ideia de como arranjar.
 *
 *  Não sei se estou a terminar as threads ordenadamente como o pessoal estava a falar mas acho que não está a haver problema nenhum?
 *  Por isso por mim esta tudo bem
 *
 *
 *  Temos que fazer isto sempre que estamos a copiar uma frase para evitar que acha um overflow no buffer
 *  e assim tambem garantimos que a frase é propriamente terminada com o \0
    strncpy(sendMsg.topico, topico, sizeof(sendMsg.topico) - 1);
    sendMsg.topico[sizeof(sendMsg.topico) - 1] = '\0';
 *
 *  Este ternario está a determinar o size do "buffer" (honestamente nem sei pq mete este nome, fui influenciado por POO)
 *  dependendo do tipo da estrutura que recebe.
 *  Tive que fazer isto pois o tamanho muda entre as menssagens e os pedidos e preciso que o buffer tenha o tamanho necessario
 *  para cada um dos dois
    char buffer[sizeof(msgStruct) > sizeof(pedidoStruct) ? sizeof(msgStruct) : sizeof(pedidoStruct)];
 *
 *  Tou a fazer isto porque senão não estava a ler bem os comandos que eram escritos no teclado, nao percebo bem o porque mas funciona
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
 *
 *
*/

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
  int tipo;                               // 0 - Inscrever ; 1 - Desinscrever ; 2 - Validar nome ; 3 - Saida ; 4 - Amostrar topicos
} pedidoStruct;

typedef struct {
  int resposta;                           // 0 - Pedido não aceite ; 1 - Pedido aceite
} respostaStruct;

typedef struct {
  char topico[20];
  int numMenssagens;
  int estado;                             // 1 - Bloqueado ; 0 - Desbloqueado
} topicosInfo;

typedef struct {
  int numTopicos;
  topicosInfo topicos[MAX_TOPICOS];
} respostaTopicosStruct;
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


