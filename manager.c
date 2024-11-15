#include "utils.h"



cliente clientesLista[MAX_CLIENTES];
int contCliente = 0;

topico topicosLista[MAX_TOPICOS];
int contTopicos = 0;

void terminaChat(int s, siginfo_t *i , void *v)
{
    unlink(MAN_FIFO);
    printf("\n[MANAGER] Terminou a sessão\n");
    exit(1);
}

void criaCliente(msgStruct *msg) 
{
  if (contCliente < MAX_CLIENTES) 
  {
    clientesLista[contCliente].pid = msg->pid;
    snprintf(clientesLista[contCliente].fifo_name, sizeof(clientesLista[contCliente].fifo_name), "/tmp/client_fifo_%d", msg->pid);
    strncpy(clientesLista[contCliente].nome , msg->nome , sizeof(clientesLista[contCliente].nome) - 1);
    clientesLista[contCliente].nome[sizeof(clientesLista[contCliente].nome) - 1] = '0';
    clientesLista[contCliente].numTopicos = 0;
    contCliente++;
  } 
  else 
  {
    printf("[ERRO] Numero maximo de clientes atingido!\n");
  }
}

void verificaAdicionaCliente(msgStruct *msg)
{
  for(int i = 0 ; i < contCliente ; i++)
  {
    if(strcmp(clientesLista[i].nome , msg->nome) == 0)
    {
      return;
    }
  }
  criaCliente(msg);
}

void criaTopico(msgStruct *msg)
{
  if(contTopicos < MAX_TOPICOS)
  {
    strncpy(topicosLista[contTopicos].nomeTopico , msg->topico , sizeof(topicosLista[contTopicos].nomeTopico) -1);
    topicosLista[contTopicos].nomeTopico[sizeof(topicosLista[contTopicos].nomeTopico) - 1] = '\0';
    topicosLista[contTopicos].contInscritos = 0;
    contTopicos++;
    printf("[MANAGER] Topico '%s' criado a pedido de um utilizador!\n" , msg->topico);
  } else 
  {
    printf("[ERRO] Numero maximo de topicos atingido!\n");
  }
}

void verificaAdicionaTopico(msgStruct *msg)
{
  for(int i = 0 ; i < contTopicos ; i++)
  {
    if(strcmp(topicosLista[i].nomeTopico , msg->topico) == 0)
    {
      return;
    }
  }
  criaTopico(msg);
}

void commRemoveCliente(char* nome) 
{
  for (int i = 0; i < contCliente; i++) 
  {
    if (strcmp(clientesLista[i].nome, nome) == 0)
    {
      clientesLista[i] = clientesLista[contCliente - 1];
      contCliente--;
      printf("[MANAGER] Cliente '%s' removido com sucesso!\n" , nome);
      return;
    }
  }
  printf("[ERRO] Cliente '%s' não encontrado!\n", nome);
}

void commUsers()
{
  //Amostra os utilizadores
  if(contCliente > 0)
  {
    printf("[MANAGER] Lista de clientes:\n");
    for(int i = 0 ; i < contTopicos ; i++)
    {
        printf("[CLIENTE] [%d] %d -> %s\n" , clientesLista[i].pid , i , clientesLista[i].nome);
    }
  } 
  else 
  {
    printf("[MANAGER] Não ha nenhum cliente registrado\n");
  }
}

void commListaTopicos()
{
  if(contTopicos > 0)
  {
    printf("[MANAGER] Lista de topicos:\n");
    for(int i = 0 ; i < contTopicos ; i++)
    {
      printf("[MANAGER] %d -> %s\n" , i , topicosLista[i]);
    }
  } 
  else 
  {
    printf("[MANAGER] Não ha nenhum topico ativo!\n");
  }
}

void commMostraTopico(char* topico)
{
}

void commBloqueiaTopico(char* topico)
{
    for(int i = 0 ; i < contTopicos ; i++)
  {
    if(strcmp(topico , topicosLista[i].nomeTopico) == 0)
    {
      if(topicosLista[i].estado == 0)
      {
        topicosLista[i].estado = 1;
        printf("[MANAGER] Topico '%s' bloqueado com sucesso!\n" , topico);
        return;
      }else
      {
        printf("[MANAGER] Topico '%s' ja bloqueado\n" , topico);
        return;
      }
    }
  }
  printf("[ERRO] Topico '%s' não encontrado na lista!\n" , topico);
}

void commDesbloqueiaTopico(char* topico)
{
  for(int i = 0 ; i < contTopicos ; i++)
  {
    if(strcmp(topico , topicosLista[i].nomeTopico) == 0)
    {
      if(topicosLista[i].estado == 1)
      {
        topicosLista[i].estado = 0;
        printf("[MANAGER] Topico '%s' desbloqueado com sucesso!\n" , topico);
        return;
      } else 
      {
        printf("[MANAGER] Topico '%s' ja desbloqueado!\n", topico);
        return;
      }
    }
  }
  printf("[ERRO] Topico '%s' não encontrado na lista!\n" , topico);
}
    //Ainda nao sei como implementar
void commFecha()
{
  for(int i = 0 ; i < contCliente ; i++){
    
  }
}



void enviaMenssagem(msgStruct *msg) 
{
  for(int i = 0 ; i < contTopicos ; i++)
  {
    if(strcmp(topicosLista[i].nomeTopico , msg->topico) == 0){
      if(topicosLista[i].estado == 1)
      {
        printf("[ERRO] Topico '%s está bloqueado!\n" , msg->topico);
        
        errorStruct errMsg;
        snprintf(errMsg.errorMenssagem, sizeof(errMsg.errorMenssagem), "Topico '%s' esta bloqueado!" , msg->topico);
        errMsg.pid = msg->pid;
        
        int fd = open(clientesLista[i].fifo_name , O_WRONLY);
        if(fd == -1)
        {
          printf("[ERRO] Falha ao abrir o FIFO do cliente '%s'!\n" , clientesLista[i].nome);
          return;
        }

        if(write(fd, &errMsg , sizeof(errMsg)) == -1)
        {
          printf("[ERRO] Falha ao enviar menssagem de erro para o cliente '%s'!\n" , clientesLista[i].nome);
        }
        close(fd);
        return;
      }
      break;
    }
  }

  for (int i = 0; i < contCliente; i++)
  {
        int inscrito = 0;
        
        for(int j = 0 ; i < clientesLista[i].numTopicos; j++)
        {
        if(strcmp(clientesLista[i].topicosIns[j] , msg->topico) == 0)
          {
          inscrito = 1;
          break;
          }
        }
        
    if(inscrito)
    {
      if(msg->duracao == 0)
      {
        int fd = open(clientesLista[i].fifo_name, O_WRONLY);
        if (fd == -1) {
            printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", clientesLista[i].pid);
            continue;
        }
        
        if (write(fd, msg, sizeof(*msg)) == -1) {
            printf("[ERRO] Falha ao enviar menssagem para o cliente %d!\n", clientesLista[i].pid);
        }

        close(fd);
      } else
      {
        //Logica para quando a menssagem for persistente

      }
    }
  }
}

void processaPedidos(pedidoStruct *pedido)
{
  for(int i = 0 ; i < contCliente ; i++)
  {
    if(clientesLista[i].pid == pedido->pid)
    {
      if(pedido->tipo == 0)
      {
        for(int j = 0 ; j < contTopicos ; j++)
        {
          if(strcmp(topicosLista[j].nomeTopico , pedido->topico) == 0)
          {
            clientesLista[i].topicosIns[clientesLista[i].numTopicos] = topicosLista[j];
            clientesLista[i].numTopicos++;
            topicosLista[j].contInscritos++;
            printf("[PEDIDO] Cliente '%s' inscrito no topico '%s'!\n" , clientesLista[i].nome , pedido->topico);
            return;
          }
        }
        printf("[ERRO-PEDIDO] Topico '%s' não encontrado!\n" , pedido->topico);
      } 
      else if(pedido->tipo == 1)
      {
        for(int j = 0 ; j < clientesLista[i].numTopicos ; j++)
        {
          if(strcmp(clientesLista[i].topicosIns[j].nomeTopico , pedido->topico) == 0)
          {
            clientesLista[i].topicosIns[j] = clientesLista[i].topicosIns[clientesLista[i].numTopicos -1];
            clientesLista[i].numTopicos--;
            printf("[PEDIDO] Cliente '%s' desinscrito do topico '%s'!\n", clientesLista[i].nome , pedido->topico);
            return;
          }
        }
        printf("[ERRO-PEDIDO] Topico '%s' não encontrado!\n", pedido->topico);
      }
      return;
    }
  }
  printf("[ERRO-PEDIDO]Cliente com PID %d não encontrado!\n", pedido->pid);
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

   /* int fd = open(MAN_FIFO, O_RDONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO!\n");
        return 1;
    }
    */

   char input[128];
    /* while(1){
        printf("Insira o comando que deseja realizar: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input , "\n")] = 0;

        if (strcmp(input, "users") == 0) {
            commUsers();
        } else if (strncmp(input, "remove ", 7) == 0) {
            char clienteNome[20];
            sscanf(input + 7, "%s", clienteNome);
            commRemoveCliente(clienteNome);
        } else if (strncmp(input, "show ", 5) == 0) {
            char topico[20];
            sscanf(input + 5, "%s", topico);
            commMostraTopico(topico);
        } else if (strcmp(input, "topics") == 0) {
            commListaTopicos();
        } else if (strncmp(input , "lock " , 5) == 0){
            char topico[20];
            sscanf(input + 5 , "%s" , topico);
            commBloqueiaTopico(topico);
        }else if(strncmp(input, "unlock " , 7) == 0){
            char topico[20]; 
            sscanf(input + 7 , "%s" , topico);
            commDesbloqueiaTopico(topico);
        } else if (strcmp(input, "close") == 0) {
            commFecha();
            break;
        } else {
            printf("[ERRO] Comando não conhecido!\n");
        }
    }
    return 0;
   }
    */ /* Acho que isto depois vai ser a main thread, a ler o teclado do manager para enviar comandos */


    /* Outra thread acho eu, so que esta é para ler o que vem do cliente */
    msgStruct recvMsg;
    while (1) 
  {
    int size = read(fd, &recvMsg, sizeof(recvMsg));
    if(size > 0 )
    {
      verificaAdicionaCliente(&recvMsg);
      verificaAdicionaTopico(&recvMsg);     
      enviaMenssagem(&recvMsg);
    }
  }
    
    close(fd);
    return 0;
}
