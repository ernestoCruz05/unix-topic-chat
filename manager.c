#include "utils.h"

int tentExit = 0;

cliente clientesLista[MAX_CLIENTES];
int contCliente = 0;

msgPersStruct menssagensLista[MAX_MENSSAGENS];
int contMenssagens = 0;

topico topicosLista[MAX_TOPICOS];
int contTopicos = 0;

void salvaMenssagens() {
    const char *f = getenv("MSG_FICH");
    if(f == NULL) {
        printf("[ERRO] Variável de ambiente MSG_FICH não definida!\n");
        return;
    }

    FILE *file = fopen(f, "w");
    if (file == NULL) {
        printf("[ERRO] Falha ao abrir o ficheiro %s para escrita!\n", f);
        return;
    }

    for (int i = 0; i < contMenssagens; i++) {
        fprintf(file, "%s %s %ld %s\n",
                menssagensLista[i].topico,
                menssagensLista[i].nome,
                menssagensLista[i].tempoVida - time(NULL),
                menssagensLista[i].menssagem);
    }

    fclose(file);
    printf("[MANAGER] Mensagens persistentes salvas no ficheiro %s.\n", f);
}

void carregaMenssagens() {
    const char *f = getenv("MSG_FICH");
    if (f == NULL) {
        printf("[ERRO] Variável de ambiente MSG_FICH não definida!\n");
        return;
    }

    FILE *file = fopen(f, "r");
    if (file == NULL) {
        printf("[ERRO] Falha ao abrir o ficheiro %s para leitura!\n", f);
        return;
    }

    char topico[100], nome[100], menssagem[300];
    long tempoVida;
    while (fscanf(file, "%s %s %ld %[^\n]", topico, nome, &tempoVida, menssagem) == 4) {
        if (contMenssagens < MAX_MENSSAGENS) {
            msgPersStruct msgNova;
            strncpy(msgNova.topico, topico, sizeof(msgNova.topico) - 1);
            msgNova.topico[sizeof(msgNova.topico) - 1] = '\0';
            strncpy(msgNova.nome, nome, sizeof(msgNova.nome) - 1);
            msgNova.nome[sizeof(msgNova.nome) - 1] = '\0';
            strncpy(msgNova.menssagem, menssagem, sizeof(msgNova.menssagem) - 1);
            msgNova.menssagem[sizeof(msgNova.menssagem) - 1] = '\0';
            msgNova.tempoVida = time(NULL) + tempoVida;

            menssagensLista[contMenssagens] = msgNova;
            contMenssagens++;
        } else {
            printf("[ERRO] Numero maximo de mensagens persistentes atingido!\n");
            break;
        }
    }

    fclose(file);
    printf("[MANAGER] Mensagens persistentes carregadas do ficheiro %s.\n", f);
}


void commFecha() {

    salvaMenssagens();

    for (int i = 0; i < contCliente; i++) {
        msgStruct termina;
        termina.fechado = 1;

        int fd = open(clientesLista[i].FIFO, O_WRONLY);
        if (fd == -1) {
            printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", clientesLista[i].pid);
            continue;
        }

        if (write(fd, &termina, sizeof(termina)) == -1) {
            printf("[ERRO] Falha ao enviar pedido de desconexão para o cliente %d!\n", clientesLista[i].pid);
        }

        close(fd);
    }

    contCliente = 0;
    printf("[MANAGER] Todos os clientes foram desconectados.\n");

    unlink(MAN_FIFO);
    printf("[MANAGER] Terminou a sessão\n");
    exit(0);
}

void handlr_sigalrm(int s , siginfo_t *i, void *v) {
    tentExit++;
    if( tentExit <= 3)
    {
        printf("\n[MANAGER] [%d / 3] Para sair usar o comando close! Apos 3 tentativas o processo ira terminar." , tentExit);
    }
    else
    {
        commFecha();
        exit(1);
    }
}

void enviaMenssagensPersistentes(cliente *client, char *topico) {
    for (int i = 0; i < contMenssagens; i++) {
        if (strcmp(menssagensLista[i].topico, topico) == 0) {
            if(menssagensLista[i].tempoVida > 0){
                msgStruct msg;
                strncpy(msg.topico, menssagensLista[i].topico, sizeof(msg.topico) - 1);
                msg.topico[sizeof(msg.topico) - 1] = '\0';
                strncpy(msg.nome, menssagensLista[i].nome, sizeof(msg.nome) - 1);
                msg.nome[sizeof(msg.nome) - 1] = '\0';
                strncpy(msg.menssagem, menssagensLista[i].menssagem, sizeof(msg.menssagem) - 1);
                msg.menssagem[sizeof(msg.menssagem) - 1] = '\0';
                msg.duracao = 0;
                msg.pid = menssagensLista[i].pid;

                int fd = open(client->FIFO, O_WRONLY);
                if (fd == -1) {
                    printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", client->pid);
                    continue;
                }

                if (write(fd, &msg, sizeof(msg)) == -1) {
                    printf("[ERRO] Falha ao enviar mensagem persistente para o cliente %d!\n", client->pid);
                }

                close(fd);
            }
        }
    }
}


void criaTopico(msgStruct *msg) {
    if (contTopicos < MAX_TOPICOS) {
        clientesLista[contCliente].pid = msg->pid;
        strncpy(topicosLista[contTopicos].nomeTopico, msg->topico, sizeof(topicosLista[contTopicos].nomeTopico) - 1);
        topicosLista[contTopicos].nomeTopico[sizeof(topicosLista[contTopicos].nomeTopico) - 1] = '\0';
        topicosLista[contTopicos].contInscritos = 0;
        contTopicos++;
        printf("[MANAGER] Topico '%s' criado a pedido de um utilizador!\n", msg->topico);
    } else {
        printf("[ERRO] Numero maximo de topicos atingido!\n");
    }
}

void verificaAdicionaTopico(msgStruct *msg) {
    for (int i = 0; i < contTopicos; i++) {
        if (strcmp(topicosLista[i].nomeTopico, msg->topico) == 0) {
            return;
        }
    }
    criaTopico(msg);
}

void commRemoveCliente(char *nome) {
    int a = 0;
    for (int i = 0; i < contCliente; i++) {
        if (strcmp(clientesLista[i].nome, nome) == 0) {
            a = 1;
            clientesLista[i] = clientesLista[contCliente - 1];

            msgStruct termina;
            termina.fechado = 1;

            int fd = open(clientesLista[i].FIFO, O_WRONLY);
            if (fd == -1) {
                printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", clientesLista[i].pid);
                continue;
            }

            if (write(fd, &termina, sizeof(termina)) == -1) {
                printf("[ERRO] Falha ao enviar pedido de desconexão para o cliente %d!\n", clientesLista[i].pid);
            }

            close(fd);


            contCliente--;
            printf("[MANAGER] Cliente '%s' removido com sucesso!\n", nome);
        }
    }
    if(!a)
    printf("[ERRO] Cliente '%s' não encontrado!\n", nome);
}

void commUsers() {
    if (contCliente > 0) {
        printf("[MANAGER] Lista de clientes:\n");
        for (int i = 0; i < contCliente; i++) {
            printf("[CLIENTE] [%d] %d -> %s\n", clientesLista[i].pid, i, clientesLista[i].nome);
        }
    } else {
        printf("[MANAGER] Não ha nenhum cliente registrado\n");
    }
}

void commListaTopicos() {
    if (contTopicos > 0) {
        printf("[MANAGER] Lista de topicos:\n");
        for (int i = 0; i < contTopicos; i++) {
            printf("[MANAGER] %d -> %s\n", i, topicosLista[i].nomeTopico);
        }
    } else {
        printf("[MANAGER] Não ha nenhum topico ativo!\n");
    }
}

void commMostraTopico(char *topico) {
    int encontrado = 0;
    for(int i = 0 ; i < contMenssagens ; i++) {
        if(strcmp(menssagensLista[i].topico, topico) == 0) {
            encontrado = 1;
            printf("\nMenssagem persistente : |<%s> %s - %s| - com duracao %d"
                , menssagensLista[i].topico , menssagensLista[i].nome , menssagensLista[i].menssagem , menssagensLista[i].tempoVida - time(NULL));
        }
    }
    if(!encontrado) {
        printf("[ERRO] Nenhuma mensagem persistente encontrada para o tópico '%s'.\n", topico);
    }
}

void commBloqueiaTopico(char *topico) {
    for (int i = 0; i < contTopicos; i++) {
        if (strcmp(topico, topicosLista[i].nomeTopico) == 0) {
            if (topicosLista[i].estado == 0) {
                topicosLista[i].estado = 1;
                printf("[MANAGER] Topico '%s' bloqueado com sucesso!\n", topico);
                return;
            } else {
                printf("[MANAGER] Topico '%s' ja bloqueado\n", topico);
                return;
            }
        }
    }
    printf("[ERRO] Topico '%s' não encontrado na lista!\n", topico);
}

void commDesbloqueiaTopico(char *topico) {
    for (int i = 0; i < contTopicos; i++) {
        if (strcmp(topico, topicosLista[i].nomeTopico) == 0) {
            if (topicosLista[i].estado == 1) {
                topicosLista[i].estado = 0;
                printf("[MANAGER] Topico '%s' desbloqueado com sucesso!\n", topico);
                return;
            } else {
                printf("[MANAGER] Topico '%s' ja desbloqueado!\n", topico);
                return;
            }
        }
    }
    printf("[ERRO] Topico '%s' não encontrado na lista!\n", topico);
}

void enviaMenssagem(msgStruct *msg) {
    verificaAdicionaTopico(msg);

    msgPersStruct msgNova;
    if (msg->duracao > 0) {
        msgNova.pid = msg->pid;
        msgNova.tempoVida = time(NULL) + msg->duracao;
        strncpy(msgNova.menssagem, msg->menssagem, sizeof(msgNova.menssagem) - 1);
        msgNova.menssagem[sizeof(msgNova.menssagem) - 1] = '\0';
        strncpy(msgNova.topico, msg->topico, sizeof(msgNova.topico) - 1);
        msgNova.topico[sizeof(msgNova.topico) - 1] = '\0';
        strncpy(msgNova.nome, msg->nome, sizeof(msgNova.nome) - 1);
        msgNova.nome[sizeof(msgNova.nome) - 1] = '\0';

        if (contMenssagens < MAX_MENSSAGENS) {
            menssagensLista[contMenssagens] = msgNova;
            contMenssagens++;
            printf("[MANAGER] Mensagem persistente adicionada ao topico '%s'!\n", msgNova.topico);
        } else {
            printf("[ERRO] Numero maximo de mensagens persistentes atingido!\n");
        }
    }

    for (int i = 0; i < contCliente; i++) {
        if (msg->pid == clientesLista[i].pid) {
            for (int j = 0; j < contTopicos; j++) {
                if (strcmp(msg->topico, clientesLista[i].topicosIns[j].nomeTopico) == 0) {
                    break;
                }
                if (strcmp(topicosLista[j].nomeTopico, msg->topico) == 0) {
                    clientesLista[i].topicosIns[clientesLista[i].numTopicos] = topicosLista[j];
                    clientesLista[i].numTopicos++;
                    topicosLista[j].contInscritos++;
                    printf("[PEDIDO] Cliente '%s' inscrito no topico '%s'!\n", clientesLista[i].nome, msg->topico);
                    enviaMenssagensPersistentes(&clientesLista[i], msg->topico);
                    break;
                }
            }
        }
    }

    for (int i = 0; i < contTopicos; i++) {
        if (strcmp(topicosLista[i].nomeTopico, msg->topico) == 0) {
            if (topicosLista[i].estado == 1) {
                printf("[ERRO] Topico '%s' está bloqueado!\n", msg->topico);
                msg->bloqueado = 1;
                snprintf(msg->menssagem, sizeof(msg->menssagem), "Topico '%s' esta bloqueado!", msg->topico);

                for (int j = 0; j < contCliente; j++) {
                    if (msg->pid == clientesLista[j].pid) {
                        int fd = open(clientesLista[j].FIFO, O_WRONLY);
                        if (fd == -1) {
                            printf("[ERRO] Falha ao abrir o FIFO do cliente '%s'!\n", clientesLista[j].nome);
                            return;
                        }

                        if (write(fd, msg, sizeof(*msg)) == -1) {
                            printf("[ERRO] Falha ao enviar mensagem de erro para o cliente '%s'!\n", clientesLista[j].nome);
                        }
                        close(fd);
                        return;
                    }
                }
            }
            break;
        }
    }

    for (int i = 0; i < contCliente; i++) {
        int inscrito = 0;
        msg->bloqueado = 0;
        for (int j = 0; j < clientesLista[i].numTopicos; j++) {
            if (strcmp(clientesLista[i].topicosIns[j].nomeTopico, msg->topico) == 0) {
                inscrito = 1;
                break;
            }
        }
        if (inscrito) {
            if (msg->duracao == 0) {
                int fd = open(clientesLista[i].FIFO, O_WRONLY);
                if (fd == -1) {
                    printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", clientesLista[i].pid);
                    continue;
                }

                if (write(fd, msg, sizeof(*msg)) == -1) {
                    printf("[ERRO] Falha ao enviar mensagem para o cliente %d!\n", clientesLista[i].pid);
                }

                close(fd);
            } else if(msg->duracao > 0) {
                enviaMenssagensPersistentes(&clientesLista[i], msg->topico);
            }
        }
    }
}

void removeCliente(pid_t pid) {
    int found = 0;
    for (int i = 0; i < contCliente; i++) {
        if (clientesLista[i].pid == pid) {
            found = 1;
        }
        if (found && i < contCliente - 1) {
            clientesLista[i] = clientesLista[i + 1];
        }
    }
    if (found) {
        contCliente--;
        printf("[MANAGER] Cliente com PID %d removido com sucesso!\n", pid);
    } else {
        printf("[ERRO] Cliente com PID %d não encontrado!\n", pid);
    }
}

void verificaTimer() {
    time_t agora = time(NULL);
    for (int i = 0; i < contMenssagens; i++) {
        if (menssagensLista[i].tempoVida <= agora) {
            printf("[MANAGER] Mensagem persistente no topico '%s' expirada e removida.\n", menssagensLista[i].topico);
            fflush(stdout);
            menssagensLista[i] = menssagensLista[contMenssagens - 1];
            contMenssagens--;
            i--;
        }
    }
}

int contaMensagensPersistentes(const char *topico) {
    int count = 0;
    for (int i = 0; i < contMenssagens; i++) {
        if (strcmp(menssagensLista[i].topico, topico) == 0) {
            count++;
        }
    }
    return count;
}


void handlePedidoTopicos(pedidoStruct *pedido) {
    respostaTopicosStruct resposta;
    resposta.numTopicos = contTopicos;

    for (int i = 0; i < contTopicos; i++) {
        strncpy(resposta.topicos[i].topico, topicosLista[i].nomeTopico, sizeof(resposta.topicos[i].topico) - 1);
        resposta.topicos[i].topico[sizeof(resposta.topicos[i].topico) - 1] = '\0';
        resposta.topicos[i].numMenssagens = contaMensagensPersistentes(topicosLista[i].nomeTopico);
        resposta.topicos[i].estado = topicosLista[i].estado;
    }

    int fd = open(pedido->FIFO, O_WRONLY);
    if (fd == -1) {
        printf("[ERRO] Falha ao abrir FIFO do cliente %d!\n", pedido->pid);
        return;
    }

    if (write(fd, &resposta, sizeof(resposta)) == -1) {
        printf("[ERRO] Falha ao enviar resposta de lista de tópicos para o cliente %d!\n", pedido->pid);
    }

    close(fd);
}


void *threadRecebeMenssagens(void *dados) {
    TDADOS *pdados = (TDADOS *) dados;
    int fd = open(MAN_FIFO, O_RDWR);
    if (fd == -1) {
        printf("\n[ERRO] Falha ao abrir o FIFO\n");
        exit(-1);
    }

    char buffer[sizeof(msgStruct) > sizeof(pedidoStruct) ? sizeof(msgStruct) : sizeof(pedidoStruct)];

    do{
            pthread_mutex_lock(pdados->m);
            int size = read(fd, buffer, sizeof(buffer));
            if (size > 0) {
                if (size == sizeof(pedidoStruct)) {
                    pedidoStruct *pedido = (pedidoStruct *)buffer;
                    if (pedido->tipo == 2) {
                        int userExists = 0;
                        for (int i = 0; i < contCliente; i++) {
                            if (strcmp(pedido->username, clientesLista[i].nome) == 0) {
                                userExists = 1;
                                break;
                            }
                        }

                        respostaStruct resposta;
                        if (userExists) {
                            printf("\n[MANAGER] Ja existe um cliente com esse nome, negar acesso!");
                            resposta.resposta = 0;
                        } else {
                            if (contCliente < MAX_CLIENTES) {
                                clientesLista[contCliente].pid = pedido->pid;
                                snprintf(clientesLista[contCliente].FIFO, sizeof(clientesLista[contCliente].FIFO), "/tmp/client_fifo_%d", pedido->pid);
                                strncpy(clientesLista[contCliente].nome, pedido->username, sizeof(clientesLista[contCliente].nome) - 1);
                                clientesLista[contCliente].nome[sizeof(clientesLista[contCliente].nome) - 1] = '\0';
                                clientesLista[contCliente].numTopicos = 0;
                                resposta.resposta = 1;
                                contCliente++;
                            } else {
                                printf("\n[ERRO] Numero de clientes maximos atingidos!");
                                resposta.resposta = 0;
                            }
                        }

                        int resp_fd = open(pedido->FIFO, O_WRONLY);
                        if (write(resp_fd, &resposta, sizeof(resposta)) == -1) {
                            printf("\n[ERRO] Falha ao enviar resposta para o fifo do feed");
                        }
                        close(resp_fd);
                    } else if(pedido->tipo == 3) {
                        removeCliente(pedido->pid);
                    } else if(pedido->tipo == 1){
                        int clientFound = 0;
                        for (int i = 0; i < contCliente; i++) {
                            if(clientesLista[i].pid == pedido->pid) {
                                clientFound = 1;
                                for(int j = 0 ; j < clientesLista[i].numTopicos ; i++) {
                                    if(strcmp(clientesLista[i].topicosIns[j].nomeTopico , pedido->topico) == 0) {
                                        for (int k = j; k < clientesLista[i].numTopicos - 1; k++) {
                                            clientesLista[i].topicosIns[k] = clientesLista[i].topicosIns[k + 1];
                                        }
                                        clientesLista[i].numTopicos--;
                                        printf("\n[MANAGER] Cliente '%d' desinscrito do topico '%s'!\n", pedido->pid, pedido->topico);
                                        break;
                                    }
                                }
                            } else {

                            }
                        }
                        if (!clientFound) {
                            printf("[ERRO] Cliente '%d' nao esta inscrito no topico '%s'!\n", pedido->pid, pedido->topico);
                        }
          } else if(pedido->tipo == 4) {
              handlePedidoTopicos(pedido);
          }else if(pedido->tipo == 0){
              for (int i = 0; i < contCliente; i++) {
                  if (clientesLista[i].pid == pedido->pid) {
                      for (int j = 0; j < clientesLista[i].numTopicos; j++) {
                          if (strcmp(clientesLista[i].topicosIns[j].nomeTopico, pedido->topico) == 0) {
                              printf("[ERRO] Cliente '%d' já está inscrito no topico '%s'!\n", pedido->pid, pedido->topico);
                              break;
                          }
                      }
                      if (clientesLista[i].numTopicos < MAX_TOPICOS) {
                          strncpy(clientesLista[i].topicosIns[clientesLista[i].numTopicos].nomeTopico, pedido->topico, sizeof(clientesLista[i].topicosIns[clientesLista[i].numTopicos].nomeTopico) - 1);
                          clientesLista[i].topicosIns[clientesLista[i].numTopicos].nomeTopico[sizeof(clientesLista[i].topicosIns[clientesLista[i].numTopicos].nomeTopico) - 1] = '\0';
                          clientesLista[i].numTopicos++;
                          printf("[MANAGER] Cliente '%d' inscrito no topico '%s'!\n", pedido->pid, pedido->topico);
                      } else {
                          printf("[ERRO] Cliente '%d' atingiu o número máximo de tópicos!\n", pedido->pid);
                      }
                      break;
                  }
              }
          }
                } else if (size == sizeof(msgStruct)) {
                    msgStruct *recvMsg = (msgStruct *)buffer;
                    enviaMenssagem(recvMsg);
                    printf("Resultado: %s - %s\n", recvMsg->nome, recvMsg->menssagem);
                }
            } else if (size == 0) {
                close(fd);
                fd = open(MAN_FIFO, O_RDONLY);
                if (fd == -1) {
                    printf("[ERRO] Falha ao reabrir FIFO!\n");
                    pthread_mutex_unlock(pdados->m);
                    return NULL;
                }
            } else {
                printf("[ERRO] Falha ao ler do FIFO!\n");
                pthread_mutex_unlock(pdados->m);
                break;
            }
            pthread_mutex_unlock(pdados->m);
    }while(pdados->stop);
    close(fd);
    pthread_exit(NULL);
}


void *threadVerificaTimer(void *dados) {
    TDADOS *pdados = (TDADOS *) dados;
    do{
        verificaTimer();
        sleep(1);
    }while(pdados->stop);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    if (argc != 1) {
        printf("[ERRO] Numero de argumentos invalido!\n");
        return 1;
    }

    struct sigaction sa;
    sa.sa_sigaction = handlr_sigalrm;
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

    carregaMenssagens();

    pthread_t recebeMenssagem, vigiaTimer;
    TDADOS v1 , v2;
    v1.stop = 1;
    v1.m = &mutex;
    v2.stop = 1;
    v2.m = &mutex;
    pthread_create(&recebeMenssagem, NULL, &threadRecebeMenssagens, &v1);
    pthread_create(&vigiaTimer, NULL , &threadVerificaTimer, &v2);
    char input[100];
    int fimComm = 0;
    do {
        printf("Insira o comando que deseja realizar: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

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
        } else if (strncmp(input, "lock ", 5) == 0) {
            char topico[20];
            sscanf(input + 5, "%s", topico);
            commBloqueiaTopico(topico);
        } else if (strncmp(input, "unlock ", 7) == 0) {
            char topico[20];
            sscanf(input + 7, "%s", topico);
            commDesbloqueiaTopico(topico);
        } else if (strcmp(input, "close") == 0) {
            commFecha();
            fimComm = 1;
        } else {
            printf("[ERRO] Comando não conhecido!\n");
        }
    } while (fimComm != 1);

    v1.stop = 0;
    v2.stop = 0;
    pthread_join(recebeMenssagem, NULL);
    pthread_join(vigiaTimer, NULL);
    pthread_mutex_destroy(&mutex);
    unlink(MAN_FIFO);
    return 0;
}
