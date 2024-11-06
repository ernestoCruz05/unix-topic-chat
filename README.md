# TP_SO_24-25
 Trabalho Pratico - Plataforma de envio e recepção de menssagens

<---------------------------------------------------------------------Objetivos------------------------------------------------------------------------->

1 -> Criar os utilizadores no programa "feed", indicando o seu nome de utilizador unico.

2 -> O utilizador pode enviar menssagens para diferentes topico, se o topico inserido pelo utilizador ainda nao existir e ainda nao tiver sido atingido o maximo de topicos possiveis é criado um esse novo topico podendo outros utilizadores se inscreverem neles. Os topicos são identificados pelo seus nomes.

3 -> Criar dois tipos de menssagens, sendo eles menssagens persistentes e menssagens não persistentes.
    3.1 -> Menssagens persistentes apos recebidas pelo servidor são guardadas por ele durante um periodo de tempo (tempo de vida) para serem redistribuid    as para novos utilizadores que entrarem no topico ou utilizadores que ja lá estavam. O tempo de vida é defenido pelo proprio utilizador que envia a m    enssagem, indicando os segundos que deseja. Se for 0 passa a ser uma menssagem não persistente.
    
    3.2 -> Menssagens não presistentes apos recebidas pelo servidor são redistribuidas para os utilizadores e eliminadas logo.


4 -> O manager guarda e busca as menssagens persistentes em ficheiro de texto. Mais a frente mais contexto

<-----------------------------------------------------------------------Feed---------------------------------------------------------------------------->

É o programa utilizado pelos utilizadores para o envio e recepção de menssages. É a interface do utlizador, sendo que todo o resto é feito pelo programa "Manager".
O programa é bloqueado ate o utilizador se identificar e a sua identificação for aceite pelo "Manager".

<----------------------------------------------------------------------Manager-------------------------------------------------------------------------->

Gera as menssagens, topicos e varios utilizadores da plataforma, interagindo com varias aplicações "Feed". Existe um utilizador especial conhecido como o"Administrador" que pode enviar varios comandos para a plataforma "Manager" para executar diversas funções como expulsar um utilizador , desligar o "Manager" entra mais coisas. Pode apenas haver um processo "Manager" ativo de cada vez.

<--------------------------------------------------------------------Utilizadores----------------------------------------------------------------------->

Existem apenas dois tipos de utilizadores:

    ->Cliente
    ->Administrador

Embora sejam ambos utilizadores têm caracteristicas diferentes.

Cliente -> Não tem a capacidade de interagir com outros utilizadores sendo as funcionalidades de envio de menssagens e subscrição em topicos feitos atraves de comandos de texto, sendo toda a informação disponibilizada texto. Criação de mais clientes é feita num novo terminal com o "Feed"

Administrador -> Controla o manager, sendo responsavel pela sua execução. Interage com a plataforma atraves do programa "Manager" com comandos que ela recebe atraves do seu stdin.

<---------------------------------------------------------------------Conteudo-------------------------------------------------------------------------->

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


