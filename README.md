# TP_SO_24-25
 Trabalho Pratico - Plataforma de envio e recepção de menssagens

<---------------------------------------------------------------------Objetivos------------------------------------------------------------------------->

1 -> Criar os utilizadores no programa "feed", indicando o seu nome de utilizador unico.

2 -> O utilizador pode enviar menssagens para diferentes topico, se o topico inserido pelo utilizador ainda nao existir e ainda nao tiver sido atingido o maximo de topicos possiveis é criado um esse novo topico podendo outros utilizadores se inscreverem neles. Os topicos são identificados pelo seus nomes.

3 -> Criar dois tipos de menssagens, sendo eles menssagens persistentes e menssagens não persistentes.
    3.1 -> Menssagens persistentes apos recebidas pelo servidor são guardadas por ele durante um periodo de tempo (tempo de vida) para serem redistribuidas para novos utilizadores que entrarem no topico ou utilizadores que ja lá estavam. O tempo de vida é defenido pelo proprio utilizador que envia a menssagem, indicando os segundos que deseja. Se for 0 passa a ser uma menssagem não persistente.
    
    3.2 -> Menssagens não presistentes apos recebidas pelo servidor são redistribuidas para os utilizadores e eliminadas logo.


4 -> O manager guarda e busca as menssagens persistentes em ficheiro de texto. Mais a frente mais contexto

<-----------------------------------------------------------------------Feed--------------------------------------------------------------------------->

É o programa utilizado pelos utilizadores para o envio e recepção de menssages. É a interface do utlizador, sendo que todo o resto é feito pelo programa "Manager".
O programa é bloqueado ate o utilizador se identificar e a sua identificação for aceite pelo "Manager".

<----------------------------------------------------------------------Manager------------------------------------------------------------------------->

Gera as menssagens, topicos e varios utilizadores da plataforma, interagindo com varias aplicações "Feed". Existe um utilizador especial conhecido como o"Administrador" que pode enviar varios comandos para a plataforma "Manager" para executar diversas funções como expulsar um utilizador , desligar o "Manager" entra mais coisas. Pode apenas haver um processo "Manager" ativo de cada vez.

<--------------------------------------------------------------------Utilizadores---------------------------------------------------------------------->

Existem apenas dois tipos de utilizadores:

    ->Cliente
    ->Administrador

Embora sejam ambos utilizadores têm caracteristicas diferentes.

Cliente -> Não tem a capacidade de interagir com outros utilizadores sendo as funcionalidades de envio de menssagens e subscrição em topicos feitos atraves de comandos de texto, sendo toda a informação disponibilizada texto. Criação de mais clientes é feita num novo terminal com o "Feed"

Administrador -> Controla o manager, sendo responsavel pela sua execução. Interage com a plataforma atraves do programa "Manager" com comandos que ela recebe atraves do seu stdin.

<---------------------------------------------------------------------Conteudo------------------------------------------------------------------------->

Limites predefindos :
    
     -> Utilizadores            : [0->10]
     -> Topicos                 : [0->20]
     -> Menssagens persistentes : [0->05] (por topico)

Menssages:
    
     -> Nome do topico [1->20] , palavra unica
     -> Conteudo da menssagem [1->300] , varias palavras

Armazenamento de menssagens:
    
     -> Se for precisso guardar menssagens em memoria, assumir os valores maximos mencionados em cima.
     -> Deve incluir a informação necessaria para cumprir os requesitos funcionais.
     -> No programa feed não é necessario guardar as menssagens.

Salvaguarda de menssagens:
    
     -> Menssagens persistentes que ainda têm tempo de vida são salvaguardadas no encerro do "Manager" sendo recuperadas quando o "Manager" re-abrir.
     -> O ficheiro de texto deve ter a seguinte estrutura:
         -> Uma menssagem por linha.
  -> As linhas devem seguir esta estrutura : <topico><username><tempo_vida><menssagem_conteudo>.
         -> O nome do ficheiro está na variavel de ambiente MSG_FICH sendo obrigatorio usa-la.

Envio de menssagens:
    
     -> A menssagem pode ter ate 300 carateres, podendo ter muito menos. Uma solução que envie apenas a menssagem com apenas o espaço necessario vai ser        mais valorizada do que uma que simplesmente envie a menssagem com o espaço para 300 carateres.

Gestão de tempo pelo manager:
    
     -> O programa "Manager" deve retirar as menssagens persistentes após terminar o seu tempo de vida.

Ficheiros de dados envolvidos:
    
     -> É necessario apenas um ficheiro, sendo esse o ficheiro onde são guardadas as menssagens persistentes. É sugerido usar as funções da biblioteca C        como o fgets , freads e etc... em vez do read & write. Mas apenas para esta funcionalidade, outras devem usar o read & write.

<--------------------------------------------------------------------Utilização------------------------------------------------------------------------>

Utilização do "Feed":
    
     -> O cliente começa o programa indicando o seu nome na linha de comandos -> "./feed Ernesto".
     -> O "Feed" so deve funcionar enquanto o manager estiver ativo, e se o manager encerrar então o "Feed" tambem deve encerrar.
     -> Apos iniciar o "Feed" com o seu username, o utilizador fica a espera de uma resposta. Se o seu username não estiver a ser usado então pode utilizar o feed para enviar comandos. Caso contrario deve receber informação do "Manager" sobre oque ocorreu.
     -> Atraves do "Feed" o utilizador pode enviar menssagens de texto e inscrever-se em diferentes topicos
     -> O manager serve como "servidor" recebendo a menssagem de um cliente e redistribuindo-as por todos os outros clientes inscritos no topico.
     -> O uso de ncurses não é obrigatorio desde que seja compreensivel o funcionamento do programa. Se tivermos tempo talvez vala a pena ver isto.

O cliente tem direito a alguns comandos:
    
     -> topics ; Deve amostrar todos os topicos, o numero de menssagens em cada um e o seu estado (bloqueado/desbloqueado).
     -> msg <topico><duração><menssagem>  ; Envio das menssagens para os clientes. A duração é sempre indicada, se for 0 é nao persistente.
     -> subscribe <topico> ; Cria um topico ou adere a esse topico. Se o topico existir o cliente adere a ele e recebe as menssagens persisntes.
     -> unsubscribe <topico> ; Desinscreve o utilizador do topico, se não houver menssagens persistentes e utilizadores, o topico é eliminado.
     -> exit ; Desconnecta o utilizador do "Manager".

Funções extra:

     -> Topicos podem ser bloqueados por administradores, sendo impossivel enviar menssagens para esse topico. Se um cliente estiver inscrito nesse topico ele é informado que o topico foi bloqueado/desbloqueado. O bloqueio de um topico não impede a inscrição nele.
     -> Um cliente pode ser expulso pelo administrador, sendo retornado de forma ordeira para a linha de comando.

Ponto de vista do administrador (Manager):
     
     -> O administrador começa o programa na lihha de comando -> "./manager".

O administrador tem acesso a alguns comandos:

     -> users ; Mostra todos os clientes connectados.
     -> remove <username> ; Remove um utilizador da plataforma, ele e todos os outros utilizadores connectados devem ser informados.
     -> topics ; Mostra os topicos e o numero de menssagens persistentes.
     -> show <topico> ; Mostra todas as menssagens persistentes de um certo topico.
     -> lock <topico> ; Bloqueia um topico.
     -> unlock <topico> ; Desbloqueia um topico.
     -> close ; Encerra a plataforma.

<--------------------------------------------------------------------Restrições------------------------------------------------------------------------>

Requisitos e restrições:

     -> Ficheiros regulares são repositorios de informação.
     -> O mecanismo de comunicação deve ser os named pipe, devendo eles serem implementados como na aula. O uso de muitos pipes é penalizado
     -> So podem ser usados mecanismos de comunicação dados nas aulas
     -> Não pode ser usado bibliotecas terceira (exceto ncurses)
     -> Não usar codigo de stackoverflow , github e etc.
     -> Todo o codigo tem que ser explicado.
