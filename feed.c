#include "utils.h"

// MAN_FIFO -> O cliente usa este FIFO para mandar menssagens para o manager.
// FED_FIFO -> Cada cliente tem o seu FED_FIFO que usa para receber menssagens do manager.

/*
 * Por implementar:
 * ncurses para deixar isto bonitos
 * talvez fazer uma menssagem especial do tipo de um DM se tiver tempo
 * void commTopicos(const char *FED_FIFO)
*/

int tentExit = 0;
char FED_FIFO[256];

void commExit(const char *FED_FIFO) {
    pedidoStruct pedido;
    pedido.pid = getpid();
    pedido.tipo = 3;

    int fd = open(MAN_FIFO, O_WRONLY);
    if (fd == -1) {
        printf("\n[ERRO] Falha ao abrir FIFO do manager!");
        return;
    }

    if (write(fd, &pedido, sizeof(pedido)) == -1) {
        printf("\n[ERRO] Falha ao enviar pedido de saída!");
    }

    close(fd);
    unlink(FED_FIFO);
    printf("\n[FEED] Terminou a sessão com sucesso");
    exit(1);
}

void handler_sigalrm(int s , siginfo_t *i, void *v)
{
  tentExit++;
  if( tentExit <= 3)
  {
    printf("\n[FEED] [%d / 3] Para sair usar o comando exit! Apos 3 tentativas o processo ira terminar." , tentExit);
  }
  else
  {
    commExit(FED_FIFO);
    exit(1);
  }
}

void handler_errorMsg(errorStruct *errMsg)
{
  printf("\n[ERRO] Recebido do manager : %s", errMsg->errorMenssagem);
}

void commLista()
{
    printf("\n[FEED] Lista de comandos:\n");
    printf("[FEED] msg <topico><duracao><menssagem>\n");
    printf("[FEED] topics \n");
    printf("[FEED] subscribe <topico>\n");
    printf("[FEED] unsubscribe <topico>\n");
    printf("[FEED] exit\n");
}

void commMsg(char *topico, int duracao, char *menssagem, char *username)
{
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
  if (fd == -1)
  {
        printf("\n[ERRO] Falha ao abrir FIFO!");
        exit(-1);
  }

  if(write(fd , &sendMsg , sizeof(sendMsg)) == -1)
  {
  printf("\n[ERRO] Falha ao enviar menssagem!");
  }

  close(fd);
}

void commTopicos(const char *FED_FIFO)
{
    //Codigo para mostrar todos os topicos criados.

    //Enviar um sinal para o manager enviar de volta os topicos.

  int fd = open(FED_FIFO, O_RDONLY);
  if (fd == -1)
  {
    printf("[ERRO] Falha ao abrir o FIFO!");
    exit(-1);
  }

  char topicos[20];
  while (read(fd, topicos, sizeof(topicos)) > 0)
  {
    printf("\n[FEED] Inscrito nos topicos : %s \n ", topicos);
  }
}

void commSubscribe(char *topico)
{
  pedidoStruct pedido;
  pedido.pid = getpid();
  strncpy(pedido.topico , topico , sizeof(pedido.topico) - 1);
  pedido.topico[sizeof(pedido.topico) - 1] = '\0';
  pedido.tipo = 0;

  int fd = open(MAN_FIFO , O_WRONLY);
  if(fd == -1)
  {
    printf("\n[ERRO] Falha ao abrir FIFO do manager!");
    return;
  }

  if(write(fd , &pedido , sizeof(pedido)) == -1)
  {
    printf("\n[ERRO] Falha ao enviar pedido de inscricao!");
  }

  close(fd);

}

void commUnsubscribe(char *topico)
{
  pedidoStruct pedido;
  pedido.pid = getpid();
  strncpy(pedido.topico , topico , sizeof(pedido.topico) - 1);
  pedido.topico[sizeof(pedido.topico) - 1] = '\0';
  pedido.tipo = 1;

  int fd = open(MAN_FIFO , O_WRONLY);
  if(fd == -1)
  {
    printf("\n[ERRO] Falha ao abrir FIFO do manager!");
    return;
  }

  if(write(fd , &pedido , sizeof(pedido)) == -1)
  {
    printf("\n[ERRO] Falha ao enviar pedido de desinscrição");
  }

  close(fd);
}

int enviaUser(char* username) {
    pedidoStruct pedido;
    respostaStruct resposta;

    pedido.pid = getpid();
    strncpy(pedido.username, username, sizeof(pedido.username) - 1);
    pedido.username[sizeof(pedido.username) - 1] = '\0';
    pedido.tipo = 2;
    snprintf(pedido.FIFO, sizeof(pedido.FIFO) , "/tmp/client_fifo_%d" , pedido.pid);

    int fd = open(MAN_FIFO, O_WRONLY);
    if (fd == -1) {
        printf("\n[ERRO] Falha ao abrir FIFO do manager!");
        exit(1);
    }

    if (write(fd, &pedido, sizeof(pedido)) == -1) {
        printf("\n[ERRO] Falha ao enviar pedido de validação!");
        close(fd);
        exit(1);
    }
    close(fd);

    int fd_feed = open(FED_FIFO, O_RDONLY);
    if (fd_feed == -1) {
        printf("\n[ERRO] Falha ao abrir FIFO do cliente!");
        exit(1);
    }

    if (read(fd_feed, &resposta, sizeof(resposta)) == -1) {
        printf("\n[ERRO] Falha ao ler resposta de validação!");
        close(fd_feed);
        exit(1);
    }
    close(fd_feed);

    if (resposta.resposta == 0) {
        printf("\n[ERRO] Username não permitido!");
        return 0;
    } else if(resposta.resposta == 3) {

    }
    return 1;
}

int nfd;
fd_set read_fds;
msgStruct msg;

int main(int argc, char *argv[])
{
    printf("\nInsira o comando que deseja realizar: ");
    fflush(stdout);
    if (argc != 2)
    {
        printf("\n[ERRO] Numero de argumentos invalidos!");
        printf("\n[FIX] Formato : %s <username> ", argv[0]);
        exit(1);
    }

    struct sigaction sa;
    sa.sa_sigaction = handler_sigalrm;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    char *username = argv[1];
    snprintf(FED_FIFO, sizeof(FED_FIFO), "/tmp/client_fifo_%d", getpid());

    if (mkfifo(FED_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("\n[ERRO] Falha ao criar FIFO do cliente!");
            return 1;
        }
    }

    int resposta = enviaUser(username);

    if(resposta == 0) {
        unlink(FED_FIFO);
        exit(-1);
    }


    int fd_man = open(MAN_FIFO, O_WRONLY);
    if (fd_man == -1)
    {
        printf("\n[ERRO] Falha ao abrir FIFO do manager!");
        return 1;
    }

    int fd_feed = open(FED_FIFO, O_RDONLY | O_NONBLOCK);
    if (fd_feed == -1)
    {
        printf("\n[ERRO] Falha ao abrir FIFO do cliente!");
        return 1;
    }

    char input[256];
    fd_set read_fds;

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(fd_feed, &read_fds);

        int max_fd = fd_feed > 0 ? fd_feed : 0;
        int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ret == -1)
        {
            printf("\n[ERRO] Falha no select!");
            break;
        }

        if (FD_ISSET(0, &read_fds))
        {
            printf("\nInsira o comando que deseja realizar: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strncmp(input, "msg ", 4) == 0)
            {
                char topico[20];
                int duracao;
                char menssagem[300];
                sscanf(input + 4, "%s %d %[^\n]", topico, &duracao, menssagem);
                commMsg(topico, duracao, menssagem, username);
            }
            else if (strcmp(input, "topics") == 0)
            {
                commTopicos(FED_FIFO);
            }
            else if (strncmp(input, "subscribe ", 10) == 0)
            {
                char topico[20];
                sscanf(input + 10, "%s", topico);
                commSubscribe(topico);
            }
            else if (strncmp(input, "unsubscribe ", 12) == 0)
            {
                char topico[20];
                sscanf(input + 12, "%s", topico);
                commUnsubscribe(topico);
            }
            else if (strcmp(input, "lista") == 0)
            {
                commLista();
            }
            else if (strcmp(input, "exit") == 0)
            {
                commExit(FED_FIFO);
                break;
            }
            else
            {
                printf("\n[ERRO] Comando não conhecido!");
            }
        }

        if (FD_ISSET(fd_feed, &read_fds))
        {
            int size = read(fd_feed, &msg, sizeof(msg));
            if (size > 0)
            {
                if(msg.fechado == 1) {
                    printf("\n[MANAGER] Manager terminado! Obrigado por participar :3");
                    fflush(stdout);
                    unlink(FED_FIFO);
                    break;
                }
                if(msg.bloqueado == 1) {
                    printf("%s" , msg.menssagem);
                    fflush(stdout);
                } else {
                    printf("\n <%s> %s - %s" , msg.topico , msg.nome , msg.menssagem);
                    fflush(stdout);
                }

            }
        }
    }

    close(fd_feed);
    close(fd_man);
    return 0;
}
