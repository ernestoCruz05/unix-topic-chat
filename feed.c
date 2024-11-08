#include "utils.h"

void terminaJogo(int s, siginfo_t *i , void *v){
  unlink(FED_FIFO);
  printf("[FEED] Terminou a sessão");
  exit(1);
}


int main(int argc, char* argv[]){

  if(argc != 2){
    printf("[ERRO] Numero de argumentos invalidos!\n");
    printf("[FIX] Formato : %s <username> \n" , argv[0]);
    exit(1);
  }

  char *username = argv[1];

  struct sigaction sa;
  sa.sa_sigaction = terminaJogo;
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigaction(SIGINT , &sa , NULL);

  if(mkfifo(FED_FIFO, 0666) != 1){
    if(errno == EEXIST){
      printf("[ERRO] O FIFO ja existe!");
    }
    printf("[ERRO] Falha ao abrir o FIFO!");
    return 1;
  }

  int fd = open(FED_FIFO , O_WRONLY);
  if(fd == -1){
    printf("[ERRO] Erro ao abrir FIFO!");
    exit(-1);
  }


  printf("Hello B)");
  return 0;
}
