#include "utils.h"

int main(int argc, char* argv[]){

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
