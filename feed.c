#include "utils.h"

// MAN_FIFO -> O cliente usa este FIFO para mandar menssagens para o manager.
// FED_FIFO -> Cada cliente tem o seu FED_FIFO que usa para receber menssagens do manager.

int tentExit = 0;
char FED_FIFO[256];

void handler_sigalrm(int s , siginfo_t *i, void *v){
  tentExit++;
  if( tentExit < 3){
    printf("\n[FEED] [%d / 3] Para sair usar o comando exit! Apos 3 tentativas o processo ira terminar.\n" , tentExit);
  } else{
    unlink(FED_FIFO);
    printf("\n[FEED] Processo terminado a forca >:(\n");
    exit(1);
    }
  }


void commLista() {
    printf("[FEED] Lista de comandos:\n");
    printf("[FEED] msg <topico><duracao><menssagem>\n");
    printf("[FEED] topics \n");
    printf("[FEED] subscribe <topico>\n");
    printf("[FEED] unsubscribe <topico>\n");
    printf("[FEED] exit");
}


void commMsg(char *topico, int duracao, char *menssagem, char *username) {
    //Codigo para enviar a menssagem para os clientes no topico.
    msgStruct sendMsg;
    // Garante que a string é propriamente terminado e que assim o buffer não da overflow
    strncpy(sendMsg.topico, topico, sizeof(sendMsg.topico) - 1);
    sendMsg.topico[sizeof(sendMsg.topico) - 1] = '\0';
    // Acaba aqui
    strncpy(sendMsg.menssagem, menssagem, sizeof(sendMsg.menssagem) - 1);
    sendMsg.menssagem[sizeof(sendMsg.menssagem) - 1] = '\0';
    strncpy(sendMsg.nome, username, sizeof(sendMsg.nome) - 1);
    sendMsg.nome[sizeof(sendMsg.nome) - 1] = '\0';
    sendMsg.duracao = duracao;
    sendMsg.pid = getpid();


    int fd = open(MAN_FIFO, O_WRONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO!\n");
        exit(-1);
    }
    close(fd);
}

void commTopicos(const char *FED_FIFO) {
    //Codigo para mostrar todos os topicos criados.

    //Enviar um sinal para o manager enviar de volta os topicos.

    int fd = open(FED_FIFO, O_RDONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir o FIFO!");
        exit(-1);
    }

    char topicos[20];
    while (read(fd, topicos, sizeof(topicos)) > 0) {
        printf("[FEED] Inscrito nos topicos : %s \n ", topicos);
    }
}

void commSubscribe(char *topico) {
    //Codigo para entrar num topico
}

void commUnsubscribe(char *topico) {
    //Codigo para desinscrever num topico
}

void commExit(const char *FED_FIFO) {
    //Codigo para sair
    unlink(FED_FIFO);
    printf("[FEED] Terminou a sessão com sucesso\n");
    exit(1);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("[ERRO] Numero de argumentos invalidos!\n");
        printf("[FIX] Formato : %s <username> \n", argv[0]);
        exit(1);
    }
  
    struct sigaction sa;
    sa.sa_sigaction = handler_sigalrm;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGINT , &sa , NULL);

    char *username = argv[1];
    snprintf(FED_FIFO, sizeof(FED_FIFO), "/tmp/client_fifo_%d", getpid());

    if (mkfifo(FED_FIFO, 0666) == -1) {
        if (errno != EEXIST) {
            printf("[ERRO] Falha ao criar FIFO do cliente!\n");
            return 1;
        }
    }

    char input[256];
    while (1) {
        printf("Insira o comando que deseja realizar: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input , "\n")] = 0;

        if (strncmp(input, "msg ", 4) == 0) {
            char topico[20];
            int duracao;
            char menssagem[300];
            sscanf(input + 4, "%s %d %[^\n]", topico, duracao, menssagem);
            commMsg(topico, duracao, menssagem, username);
        } else if (strcmp(input, "topics") == 0) {
            commTopicos(FED_FIFO);
        } else if (strncmp(input, "subscribe ", 10) == 0) {
            char topico[20];
            sscanf(input + 10, "%s", topico);
            commSubscribe(topico);
        } else if (strncmp(input, "unsubscribe ", 12) == 0) {
            char topico[20];
            sscanf(input + 12, "%s", topico);
            commUnsubscribe(topico);
        } else if (strcmp(input, "lista") == 0) {
            commLista();
        } else if (strcmp(input, "exit") == 0) {
            commExit(FED_FIFO);
            break;
        } else {
            printf("[ERRO] Comando não conhecido!\n");
        }
    }
    return 0;
}
