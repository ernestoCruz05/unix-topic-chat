#include "utils.h"


void recebeFeedMenssagem(char *FIFO){
  int fd = open(FIFO, O_RDONLY);
  if(fd == -1){
    printf("[ERRO] Falha ao abrir FIFO!\n");
    exit(-1);
  }
  
  msgStruct fd_msg;
  while(read(fd , fd_msg , sizeof(fd_msg)) > 0){
    printf("[%d] %s -> %s" , fd_msg.pid , fd_msg.nome , fd_msg.menssagem);
  }

  close(fd);
}


int main(int argc, char* argv[]){

  char listaComandos[7][10] = {"users" , "remove" , "topic" , "show" , "lock" , "unlock" , "close"};

  msgStruct fd_msg;

  if(argc != 1){
    printf("[ERRO] Numero de argumentos invalido!\n");
    return(1);
  }


  if(mkfifo(MAN_FIFO, 0666) == -1){
    if(errno == EEXIST){
      printf("[ERRO] O FIFO ja existe!\n");
      return 1;
    }
    printf("[ERRO] Falha ao abrir o FIFO!\n");
    return 1;
  }

  mkfifo(MAN_FIFO, 0666);
  fd_msg = open(MAN_FIFO , O_RDWR);

}
