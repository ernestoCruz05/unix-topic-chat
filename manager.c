#include "utils.h"



cliente clientes[MAX_CLIENTES];
int contCliente = 0;

topico topicos[MAX_TOPICOS];
int contTopicos = 0;

void terminaChat(int s, siginfo_t *i , void *v){
    unlink(MAN_FIFO);
    printf("\n[MANAGER] Terminou a sessão\n");
    exit(1);
}

void criaCliente(int pid) {
    if (contCliente < MAX_CLIENTES) {
        clientes[contCliente].pid = pid;
        snprintf(clientes[contCliente].fifo_name, sizeof(clientes[contCliente].fifo_name), "/tmp/client_fifo_%d", pid);
        contCliente++;
    } else {
        printf("[ERRO] Numero maximo de clientes atingido!\n");
    }
}

void criaTopico(msgStruct *msg){
  if(contTopicos < MAX_TOPICOS){
    strcpy(topicos[contTopicos].nomeTopico, msg.topico);
    snprintf(topicos[contTopicos].fifo_name , sizeof(topicos[contTopicos].fifo_name), "/tmp/clientes_fifo_%s" , nomeTopico);
    contTopicos++;
  } else {
    printf("[ERRO] Numero maximo de topicos atingido!\n");
  } 
}

void removeCliente(char* nome) {
    for (int i = 0; i < contCliente; i++) {
        if (strcmp(clientes[i].nome, nome) == 0) {
            clientes[i] = clientes[contCliente - 1];
            contCliente--;
            break;
        }
    }
}

void commUsers(){
  //Amostra os utilizadores
}

void commTopicosDetalhado(){

}

void commMostraTopico(char* topico){

}

void commBloqueiaTopico(char* topico){

}

void commDesbloqueiaTopico(char* topico){
  
}

void commFecha(){

}



void broadcastMessage(msgStruct *msg) {
    for (int i = 0; i < contCliente; i++) {
        int fd = open(clientes[i].fifo_name, O_WRONLY);
        if (fd == -1) {
            printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", clientes[i].pid);
            continue;
        }

        if (write(fd, msg, sizeof(*msg)) == -1) {
            printf("[ERRO] Falha ao enviar menssagem para o cliente %d!\n", clientes[i].pid);
        }

        close(fd);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 1) {
        printf("[ERRO] Numero de argumentos invalido!\n");
        return 1;
    }

    struct sigaction sa;
    sa.sa_sigaction = terminaChat;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    if (mkfifo(MAN_FIFO, 0666) == -1) {
        if (errno == EEXIST) {
            printf("[ERRO] O FIFO ja existe! A eliminar FIFO...");
            unlink(MAN_FIFO);
            if (mkfifo(MAN_FIFO, 0666) == -1) {
                printf("[ERRO] Falha ao recriar FIFO!\n");
                return 1;
            }
        } else {
            printf("[ERRO] Falha ao abrir o FIFO!\n");
            return 1;
        }
    }

    int fd = open(MAN_FIFO, O_RDONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO!\n");
        return 1;
    }

    msgStruct recvMsg;
    while (1) {
        int size = read(fd, &recvMsg, sizeof(recvMsg));
        if (size > 0) {
            printf("[%d] %s -> %s\n", recvMsg.pid, recvMsg.nome, recvMsg.menssagem);
            fflush(stdout);
            addClient(recvMsg.pid); // Add client to the list
            broadcastMessage(&recvMsg); // Broadcast message to all clients
        }
    }

    close(fd);
    return 0;
}
