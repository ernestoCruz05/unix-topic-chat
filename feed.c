#include "utils.h"


void commMsg(char *topico, int duracao , char* menssagem){
  //Codigo para enviar a menssagem para os clientes no topico.

}

void commTopicos(){
  //Codigo para mostrar todos os topicos criados.
  
  //Enviar um sinal para o manager enviar de volta os topicos.

  int fd = open(FED_FIFO, O_RDONLY);
  if(fd == -1){
    printf("[ERRO] Falha ao abrir o FIFO!");
    exit(-1);
  }
  
  char topicos[20]
  while(read(fd, topicos, sizeof(topicos)) > 0){
    printf("[FEED] Inscrito nos topicos : %s \n " , topicos);
  }

}

void commSubscribe(char *topico){
  //Codigo para entrar num topico
}

void commUnsubscribe(char *topico){
  //Codigo para desinscrever num topico
}

void commExit(){
  //Codigo para sair
  char FED_FIFO[128];
  snprintf(FED_FIFO, sizeof(FED_FIFO), "/tmp/client_fifo_d", getpid());
  unlink(FED_FIFO);
  printf("[FEED] Terminou a sessão com sucesso\n");
  exit(1);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[ERRO] Numero de argumentos invalidos!\n");
        printf("[FIX] Formato : %s <username> \n", argv[0]);
        exit(1);
    }

    char *username = argv[1];
    char FED_FIFO[256];
    snprintf(FED_FIFO, sizeof(FED_FIFO), "/tmp/client_fifo_%d", getpid());

    if (mkfifo(FED_FIFO, 0666) == -1) {
        if (errno != EEXIST) {
            printf("[ERRO] Falha ao criar FIFO do cliente!\n");
            return 1;
        }
    }

    msgStruct sendMsg;
    sendMsg.pid = getpid();
    strcpy(sendMsg.nome, username);
    strcpy(sendMsg.menssagem, "Ola isto é um teste");

    int fd = open(MAN_FIFO, O_WRONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO!\n");
        exit(-1);
    }

    if (write(fd, &sendMsg, sizeof(sendMsg)) == -1) {
        printf("[ERRO] Falha ao enviar menssagem\n");
        close(fd);
        exit(-1);
    }

    close(fd);

    // Abrir FIFO do cliente para leitura
    fd = open(FED_FIFO, O_RDONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO do cliente!\n");
        return 1;
    }

    char input[256];
    while (1) {
      printf("Insira o comando que deseja realizar: ");
      fgets(input , sizeof(input) , stdin);
      
      if(strncmp(input, "msg ", 4) == 0){
        char topico[20];
        int duracao;
        char menssagem[300];
        sscanf(input + 4 , "%s %d %[^\n]" , topico , duracao , menssagem);
        commMsg(topico, duracao , menssagem);
      } else if(strcmp(input , "topics") == 0){
        commTopicos();
      } else if(strncmp(input , "subscribe " , 10 ) == 0){
        char topico[20];
        sscanf(input + 10 , "%s" , topico);
        commSubscribe(topico);
      } else if(strncmp (input, "unsubscribe " , 12) == 0){
        char topico[20];
        sscanf(input + 12 , "%s" , topico);
        commUnsubscribe(topico);
      } else if(strcmp(input , "exit") == 0){
        commExit();
        break;
      } else {
        printf("[ERRO] Comando não conhecido!");
      }
    }

    close(fd);
    return 0;
}
