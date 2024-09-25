% Alexandre Carapeto Delgado 109441
:-use_module(library(clpfd)). %para poder usar transpose/2
:-set_prolog_flag(answer_write_options,[max_depth(0)]). %ver listas completas
:-['puzzlesAcampar.pl']. %Ficheiro dado. No Mooshak tera mais puzzles.
% Atencao: nao deves copiar nunca os puzzles para o teu ficheiro de codigo
% Segue-se o codigo



/*==============================
Predicado Auxiliar celulaLivre/2

E verdade se Tabuleiro e um tabuleiro e a celula (Lin,Col) nao contem nenhum objecto
O predicado esta feito especificamente para as situacoes em que me da jeito usa-lo,
logo nao esta preparado para receber celulas fora do tabuleiro, por exemplo


Usado como auxiliar em:
todasCelulas/3
celulaVazia/2
insereObjectoCelula/3
celulasAproveitaveis/2 (predicado auxiliar)
vizinhancasLivresArvores/2 (predicado auxiliar) */


celulaLivre(Tabuleiro, (Lin, Col)):-
	nth1(Lin, Tabuleiro, LinCompleta),
	nth1(Col, LinCompleta, Livre),
	%So ira passar se a celula contiver uma variavel 
	var(Livre).

%===============================



/*=================================
Predicado Auxiliar conteudoCelula/3

E verdade se Tabuleiro e um tabuleiro e Conteudo e o conteudo da celula (Lin,Col)
Feito para obter o conteudo duma celula e nao estar a repetir estas duas linhas ao longo do codigo todo

O primeiro nth1 devolve a lista correspondente a coordenada da linha e
o segundo nth1 combina-a com a coluna para obter o elemento

Usado como auxiliar em:
todasCelulas/3
celulaVazia/2 */


conteudoCelula(Tabuleiro, (Lin, Col), Conteudo):-
	nth1(Lin, Tabuleiro, LinCompleta),
	nth1(Col, LinCompleta, Conteudo). 

%==================================



/*====================
Predicado vizinhanca/2

Usado como auxiliar em:
acessiveis/2
descartaArvoresNaoIsoladas/3 (predicado auxiliar)
vizinhancasLivresArvores/2 (predicado auxiliar) */


vizinhanca((Lin,Col), [(LinDeBaixo,Col), (Lin,ColEsquerda), (Lin,ColDireita), (LinDeCima,Col)]):-
	LinDeBaixo is Lin-1,
	ColEsquerda is Col-1,
	ColDireita is Col+1,
	LinDeCima is Lin+1.

%=====================



/*============================
Predicado vizinhancaAlargada/2

Usado como auxiliar em:
todasVizinhancasTendas/2 (predicado auxiliar) */

vizinhancaAlargada((Lin,Col),
		   [(LinDeBaixo,ColEsquerda), (LinDeBaixo,Col), (LinDeBaixo,ColDireita),
		    (Lin,ColEsquerda), (Lin,ColDireita),
		    (LinDeCima,ColEsquerda), (LinDeCima,Col), (LinDeCima,ColDireita)]):-
	
	LinDeBaixo is Lin-1,
	ColEsquerda is Col-1,
	ColDireita is Col+1,
	LinDeCima is Lin+1.


%=============================



/*=====================================
Predicado Auxiliar geraListaCrescente/2

geraListaCrescente/2 e um predicado auxiliar que dado um inteiro gera uma lista de numeros
por ordem crescente ate esse inteiro (inclusive)

Usado como auxiliar em:
todasCelulas/2
todasCelulas/3
celulasAproveitaveis/2 (predicado auxiliar) */

geraListaCrescente(0, []):- !.

geraListaCrescente(Num, ListaNums):-
	NovoNum is Num-1,
	geraListaCrescente(NovoNum, NovaListaNums),
	
	%vai-se adicionando o numero Num (que e maior que NovoNum) como sufixo
	%para a lista ficar por ordem crescente
	append(NovaListaNums,[Num],ListaNums), !.

%==========================



/*======================
Predicado todasCelulas/2

Usado como auxiliar em:
inacessiveis/1

Obtenho a ordem da matriz Tabuleiro (Ord) e crio uma lista dos numeros ate esse numero, por ordem crescente
Uso essa lista para obter todas as combinacoes possiveis de dois numeros nessa lista */
todasCelulas(Tabuleiro, TodasCelulas):-
	length(Tabuleiro, Ord),
	geraListaCrescente(Ord, ListaNums),
	findall( (Lin,Col), (member(Lin,ListaNums),member(Col,ListaNums)), TodasCelulas).

%=======================



/*======================
Predicado todasCelulas/3

Usado como auxiliar em:
inacessiveis/1
limpaVizinhancas/1
unicaHipotese/1 */


%Caso particular para uma constante
todasCelulas(Tabuleiro, TodasCelulas, Objecto):-
	not(var(Objecto)),
	length(Tabuleiro, Ord),
	geraListaCrescente(Ord, ListaNums),
	
	findall( (Lin,Col),

	%Estes dois member/2 geram as combinacoes de coordenadas
	(member(Lin,ListaNums),member(Col,ListaNums),
	
	conteudoCelula(Tabuleiro, (Lin, Col), Conteudo),
	%Conteudo tem que ser o objecto que estamos a procura
	Conteudo==Objecto),

	TodasCelulas).


%Caso particular para uma variavel
todasCelulas(Tabuleiro, TodasCelulas, Objecto):-
	var(Objecto),
	length(Tabuleiro, Ord),
	geraListaCrescente(Ord, ListaNums),
	
	findall( (Lin,Col),
	
	%Estes dois member/2 geram as combinacoes de coordenadas
	(member(Lin,ListaNums),member(Col,ListaNums),
	
	%Procuramos as celulas que nao tem nenhum objecto
	celulaLivre(Tabuleiro, (Lin,Col))),
	
	TodasCelulas).	

%=======================



/*==============================================
Predicado Auxiliar calculaObjectosTabuleiroLinhas/3

Usado como auxiliar em:
calculaObjectosTabuleiro/4

Inicialmente fiz este predicado para calcular os objectos nas linhas, por isso os nomes das variaveis estao de acordo com esse proposito
Mas basta passar o tabuleiro transposto para calcular os objectos nas colunas do tabuleiro original */

calculaObjectosTabuleiroLinhas([], [], _).
calculaObjectosTabuleiroLinhas([PrimeiraLin|Resto], ContagemLinhas, Objecto):-
	not(var(Objecto)),
	findall(_, (nth1(_,PrimeiraLin,Conteudo), Conteudo==Objecto), ListaObjectos),
	
	%O comprimento da ListaObjectos da-nos o numero de vezes que o objecto aparece
	length(ListaObjectos, NumObjectos),
	calculaObjectosTabuleiroLinhas(Resto, ContagemLinhasNova, Objecto),
	append([NumObjectos], ContagemLinhasNova, ContagemLinhas).



calculaObjectosTabuleiroLinhas([PrimeiraLin|Resto], ContagemLinhas, Objecto):-
	var(Objecto),
	findall(_, (nth1(_,PrimeiraLin,content)), ListaObjectos),
	
	length(ListaObjectos,NumObjectos),
	calculaObjectosTabuleiroLinhas(Resto, ContagemLinhasNova, Objecto),
	append([NumObjectos], ContagemLinhasNova, ContagemLinhas).

%================================================



/*==================================
Predicado calculaObjectosTabuleiro/4

Usado como auxiliar em:
relvaLinhas/3
celulasAproveitaveis/2
resolvido/1 */

calculaObjectosTabuleiro(Tabuleiro, ContagemLinhas, ContagemColunas, Objecto):-
	calculaObjectosTabuleiroLinhas(Tabuleiro, ContagemLinhas, Objecto),
	
	transpose(Tabuleiro,TabuleiroTrans),
	calculaObjectosTabuleiroLinhas(TabuleiroTrans, ContagemColunas, Objecto).

%====================================



/*==============================
Predicado Auxiliar naoPertence/2

naoPertence/2 e um predicado auxiliar que e verdade se as coordenadas (Lin,Col) nao pertencerem ao Tabuleiro de ordem Ord

Usado como auxiliar em:
celulaVazia/2
todasVizinhancasTendas/2 (predicado auxiliar) */

naoPertence((Lin,Col), Ord):-
	Lin > Ord;
	Lin < 1;
	
	Col > Ord;
	Col < 1.

%===============================



/*=====================
Predicado celulaVazia/2


Se as coordenadas nao fizerem parte do tabuleiro, o predicado nao deve falhar
Ou seja, adiciono este caso onde o predicado tambem se verifica se a celula nao pertencer ao tabuleiro */
celulaVazia(Tabuleiro, (Lin,Col)):-
	%Ord e a ordem da matriz Tabuleiro
	length(Tabuleiro, Ord),
	naoPertence((Lin,Col), Ord), !.


%Aqui verifica-se se a celula contem relva
celulaVazia(Tabuleiro, (Lin,Col)):-
	conteudoCelula(Tabuleiro, (Lin,Col), Conteudo),
	Conteudo == r.


%Aqui verifica-se se a celula contem uma variavel.
%Se nao contiver relva nem uma variavel, entao nao e vazia e da false
celulaVazia(Tabuleiro, (L,C)):-
	celulaLivre(Tabuleiro, (L,C)).

%=======================



/*=========================
Predicado Auxiliar insere/5

insere(ListaInicial, ListaFinal, NovoElem, Indice, Contador)

Predicado auxiliar que insere elemento NovoElem na posicao Indice da lista
(para os indices comecarem em 1, chamar predicado com o Contador em 1)

Usado como auxiliar em:
insereObjectoCelula/3 */


%Caso terminal e quando ja nao ha mais lista para analisar
insere([],[],_,_,_).

%Se estivermos na posicao da lista onde queremos inserir
%(o contador diz-nos a posicao da lista em que estamos e o indice a posicao que queremos)
insere([_|Resto], Lista, NovoElem, Indice, Contador):-
	Indice==Contador,
	ProxContador is Contador + 1,

	insere(Resto, NovaLista, NovoElem, Indice, ProxContador),
	
	%Pomos o NovoElem no lugar do elemento em que estamos
	Lista = [NovoElem|NovaLista].


%Se nao estivermos na posicao onde queremos inserir, passamos para a proxima
insere([ElemAManter|Resto], Lista, NovoElem, Indice, Contador):-
	Indice\==Contador,
	ProxContador is Contador + 1,

	insere(Resto, NovaLista, NovoElem, Indice, ProxContador),
	
	%Mantemos o elemento em que estamos	
	Lista = [ElemAManter|NovaLista].

%==========================



/*=============================
Predicado insereObjectoCelula/3

Usado como auxiliar em:
insereObjectoEntrePosicoes/4
inacessiveis/1
celulasAproveitaveis/2 (predicado auxiliar)
limpaVizinhancas/1
insereNaUnicaHipotese/3 (predicado auxiliar)
resolveTentativa/3 (predicado auxiliar) */

%Se a celula contiver um objecto (mesmo que seja relva), nao esta livre, entao nada e inserido
insereObjectoCelula(Tabuleiro, _, (Lin,Col)):-
	not(celulaLivre(Tabuleiro, (Lin,Col))), !.


%Se a celula estiver livre
insereObjectoCelula(Tabuleiro, TendaOuRelva, (Lin,Col)):-	
	nth1(Lin, Tabuleiro, LinCompleta),
	%Vamos a linha no tabuleiro e inserimos na coluna
	insere(LinCompleta, NovaLinha, TendaOuRelva, Col, 1),
	%Vamos ao tabuleiro e inserimos a linha modificada
	insere(Tabuleiro, Tabuleiro, NovaLinha, Lin, 1).

%==============================



%======================================
%Predicado insereObjectoEntrePosicoes/4


%Se a Col1 for maior que a Col2, entao troco as duas variaveis para a nova Col1 ser menor que a nova Col2
insereObjectoEntrePosicoes(Tabuleiro, TendaOuRelva, (Lin,Col1), (Lin,Col2)):-
	Col1 > Col2,
	insereObjectoEntrePosicoes(Tabuleiro, TendaOuRelva, (Lin,Col2), (Lin,Col1)).


%Se a Col1 for menor que a Col2, insiro o objecto na celula da Col1 e chamo o predicado de novo,
%avancando uma coluna para a direita (aproximo-me da Col2)
insereObjectoEntrePosicoes(Tabuleiro, TendaOuRelva, (Lin,Col1), (Lin,Col2)):-
	Col1 < Col2,
	insereObjectoCelula(Tabuleiro, TendaOuRelva, (Lin,Col1)),
	
	NextCol is Col1 + 1,
	insereObjectoEntrePosicoes(Tabuleiro, TendaOuRelva, (Lin,NextCol), (Lin,Col2)).


%Se a Col1 e a Col2 forem iguais, entao esta e a ultima celula onde quero inserir.
%Chamo o predicado para inserir e nao faco mais nada
insereObjectoEntrePosicoes(Tabuleiro, TendaOuRelva, (Lin,Col1), (Lin,Col2)):-
	Col1 == Col2,
	insereObjectoCelula(Tabuleiro, TendaOuRelva, (Lin,Col1)), !.

%======================================



/*==============================
Predicado Auxiliar avaliaRelva/5

Este predicado apenas serve para nao repetir codigo no relvaLinhas/3
O relvaLinhas/3 fornece a informacao das tendas supostas e existentes para este predicado decidir
se deve inserir relva ou nao

Usado como auxiliar em:
relvaLinhas/3 */


avaliaRelva(Tabuleiro, TendasSupostas, TendasExistentes, Linha, ColMax):-
	%Se, na linha que estamos a analisar, existem tantas tendas quanto e suposto
	TendasSupostas == TendasExistentes,

	%Entao quero inserir relva na linha toda (escolho as duas celulas nos extremos da linha)
	insereObjectoEntrePosicoes(Tabuleiro, r, (Linha,1), (Linha,ColMax))
	
	%OU
	;
	%Se nao existe o numero de tendas que e suposto, nao faco nada e
	%o relvaLinhas/1 avanca para a proxima linha	
	TendasSupostas \= TendasExistentes, !.

%===============================



/*==============================
Predicado Auxiliar relvaLinhas/3

relvaLinhas/3 e um predicado auxiliar que tem o mesmo proposito que o predicado relva/1 que e pedido,
mas apenas coloca relva nas linhas. Para isto, contudo, precisa de chamar o predicado avaliaRelva/5

Usado como auxiliar em:
relva/1


Caso terminal: se o contador Linha ultrapassar a ColMax, estariamos a analisar uma linha inexistente,
logo terminamos o programa */



%Isto funciona pois Tabuleiro e uma matriz quadrada
relvaLinhas(Tabuleiro, _, Linha):-
	length(Tabuleiro, ColMax),
	Linha > ColMax, !.


%Linha e um contador que e usado como indice nas listas de numeros de tendas
relvaLinhas(Tabuleiro,  TendasSupostasLinhas, Linha):-
	length(Tabuleiro, ColMax),

	%Da-nos quantas tendas existem na realidade
	calculaObjectosTabuleiro(Tabuleiro, TendasExistentesLinhas, _, t),

	nth1(Linha, TendasSupostasLinhas, TendasSupostas),
	nth1(Linha, TendasExistentesLinhas, TendasExistentes),
	
	%Avalio se se deve inserir relva ou nao
	avaliaRelva(Tabuleiro, TendasSupostas, TendasExistentes, Linha, ColMax),

	%Analiso a proxima linha
	ProxLinha is Linha + 1,
	relvaLinhas(Tabuleiro, TendasSupostasLinhas, ProxLinha).


%===============================



/*===============
Predicado relva/1

Usado como auxiliar em:
estrategiasPrincipais/1


Depois de inserir a relva que posso nas linhas, transponho o tabuleiro e repito o processo para inserir nas colunas */

relva( (Tabuleiro, TendasSupostasLinhas, TendasSupostasColunas) ):-
	relvaLinhas(Tabuleiro, TendasSupostasLinhas, 1),

	transpose(Tabuleiro, TabuleiroTrans),
	relvaLinhas(TabuleiroTrans, TendasSupostasColunas, 1).

%=================



/*============================
Preciado Auxiliar acessiveis/2

acessiveis/2 e um predicado auxiliar em que:
TodasArvores e uma lista de celulas;
Acessiveis e uma lista com todas as celulas que pertencem a pelo menos uma vizinhanca das celulas fornecidas

Usado como auxiliar em:
inacessiveis/1

Como este predicado nos auxilia no predicado inacessiveis/1, os nomes das variaveis
foram dados com arvores em mente, e decidi manter assim por clareza */
acessiveis(TodasArvores, Acessiveis):-
	findall(Vizinhanca,
	%Por cada arvore do tabuleiro (presente na lista de TodasArvores)
	(member(Arvore, TodasArvores),
	%quero calcular a sua vizinhanca
	vizinhanca(Arvore, Vizinhanca)),
	%e junta-las todas numa lista
	TodasVizinhancas),
	
	%TodasVizinhancas e uma lista de listas, mas torno-a apenas numa lista	
	append(TodasVizinhancas, Acessiveis).

%=============================



/*======================
Predicado inacessiveis/1

Usado como auxiliar em:
resolve/2 (predicado auxiliar) */

inacessiveis(Tabuleiro):-
	%Obtenho todas as celulas com arvores e depois todas as celulas das suas vizinhancas
	todasCelulas(Tabuleiro, TodasArvores, a),
	acessiveis(TodasArvores, Acessiveis),
	
	%De todas as celulas do tabuleiro, as inacessiveis sao
	%todas aquelas que nao pertencem a lista Acessiveis
	todasCelulas(Tabuleiro, TodasCelulas),
	findall((Lin,Col),
	(member((Lin,Col), TodasCelulas), not(member((Lin,Col), Acessiveis))),
	Inacessiveis),
	
	%Insiro relva em todas as celulas inacessiveis
	maplist(insereObjectoCelula(Tabuleiro,r),Inacessiveis).


%========================



/*=======================================
Predicado Auxiliar celulasAproveitaveis/2

celulasAproveitaveis/2 e um predicado auxiliar que, dado um tabuleiro,
calcula todas as celulas aproveitaveis (que sao celulas livres)

Por celulas aproveitaveis entenda-se todas as celulas livres das linhas onde
o numero de tendas que faltam inserir e igual ao numero de celulas livres

Usado como auxiliar em:
aproveita/1 */

celulasAproveitaveis(Tabuleiro, TendasSupostasLinhas):-
	%Ord e a ordem da matriz Tabuleiro
	length(Tabuleiro, Ord),
	%ListaNums e uma lista que contem todos os inteiros de 1 ate Ord (inclusive)
	geraListaCrescente(Ord, ListaNums),

	calculaObjectosTabuleiro(Tabuleiro, TendasExistentesLinhas, _, t),
	calculaObjectosTabuleiro(Tabuleiro, CelulasLivresLinhas, _, _),

	%Este findall gera uma lista com todas as celulas aproveitaveis e e nelas que quero colocar tendas
	findall((Lin,Col),
	(member(Lin, ListaNums),
	
	%Obtenho quantas tendas e suposto existirem e quantas tendas existem para cada linha 
	nth1(Lin, TendasSupostasLinhas, TendasSupostas),
	nth1(Lin, TendasExistentesLinhas, TendasExistentes),

	FaltaInserir is TendasSupostas - TendasExistentes,
	
	nth1(Lin, CelulasLivresLinhas, CelulasLivres),
	%Se faltam inserir tantas tendas quanto as celulas livres, entao a linha e aproveitavel
	FaltaInserir == CelulasLivres,
	
	%Entao vou buscar todas as colunas e gero todas as coordenadas que contem uma celula livre
	member(Col, ListaNums),
	celulaLivre(Tabuleiro, (Lin,Col))),
	
	CelAproveitaveis),

	%Insere uma tenda em todas as celulas aproveitaveis
	maplist(insereObjectoCelula(Tabuleiro,t),CelAproveitaveis).

%========================================



/*===================
Predicado aproveita/1

Usado como auxiliar em:
estrategiasPrincipais/1 (predicado auxiliar)

Analiso as linhas e ponho tendas nas celulas aproveitaveis, transponho o tabuleiro
e faco o mesmo para as colunas */
aproveita( (Tabuleiro, TendasSupostasLinhas, TendasSupostasColunas) ):-
	celulasAproveitaveis(Tabuleiro, TendasSupostasLinhas),

	transpose(Tabuleiro, TabuleiroTrans),
	celulasAproveitaveis(TabuleiroTrans, TendasSupostasColunas).
%====================



/*=========================================
Predicado Auxiliar todasVizinhancasTendas/2

Este predicado gera uma lista com todas as celulas que pertencem a, pelo menos,
uma vizinhanca alargada duma tenda

Usado como auxiliar em:
limpaVizinhancas/1
resolvido/1 (predicado auxiliar) */

todasVizinhancasTendas(Tabuleiro, TodasVizinhancasTendas):-
	length(Tabuleiro, Ord),

	todasCelulas(Tabuleiro, TodasTendas, t),

	%Calculo a vizinhanca alargada para cada tenda que existe e junto-as todas numa lista
	findall(VizinhosTenda,
	(member(Tenda, TodasTendas),
	vizinhancaAlargada(Tenda, VizinhosTenda)),	
	TodasVizinhancasNaoPronta),
	

	%TodasVizinhancasNaoPronta e uma lista que nao esta pronta para ser usada para o que queremos pois	
	%e uma lista de listas e, por isso, corrigimos isso
	append(TodasVizinhancasNaoPronta, TodasVizinhancasAlisadas),

	%e inclui celulas que nao pertencem ao tabuleiro, e corrigimos isso tambem
	findall(Celula,
	(member(Celula, TodasVizinhancasAlisadas), not(naoPertence(Celula, Ord))),
	TodasVizinhancasTendas).

%==========================================



/*==========================
Predicado limpaVizinhancas/1

Usado como auxiliar em:
estrategiasPrincipais/1 (predicado auxiliar)
resolveTentativa/3 (predicado auxiliar) */

limpaVizinhancas( (Tabuleiro, _, _)):-
	todasVizinhancasTendas(Tabuleiro, TodasVizinhancasTendas),

	%insiro relva em todas as celulas que pertencem a pelo menos uma vizinhanca alargada duma tenda
	maplist(insereObjectoCelula(Tabuleiro,r),TodasVizinhancasTendas).

%===========================



/*=============================================
Predicado Auxiliar descartaArvoresNaoIsoladas/3

descartaArvoresNaoIsoladas/3 e um predicado auxiliar que, dada uma lista de arvores,
remove aquelas que tem uma ou mais tendas na sua vizinhanca

Usado como auxiliar em:
unicaHipotese/1
valida/2 */


%Caso terminal quando ja nao ha mais arvores a analisar (lista e vazia)
descartaArvoresNaoIsoladas(_,[],[]).

descartaArvoresNaoIsoladas(TodasTendas, [Arvore|AsOutrasArvores], ArvoresIsoladas):-
	vizinhanca(Arvore, Vizinhanca),

	%Quero as celulas que pertencem simultaneamente a vizinhanca da arvore e
	%a lista de todas as tendas (ou seja, celulas que tenham uma tenda)	
	findall(Celula,
	(member(Celula, Vizinhanca),
	member(Celula, TodasTendas)),
	TendasNaVizinhanca),

	%Se nao houver tendas (lista e vazia, tem comprimento 0), entao
	%mantenho esta arvore e chamo o mesmo predicado para analisar a proxima
	length(TendasNaVizinhanca, TemTendas),
	TemTendas == 0,
	descartaArvoresNaoIsoladas(TodasTendas, AsOutrasArvores, ArvoresIsoladasNew),
	ArvoresIsoladas = [Arvore|ArvoresIsoladasNew], !

	%OU, se a lista nao for vazia, ou seja, houver pelo menos
	%uma tenda na vizinhanca da arvore, descarto essa arvore e passo para a proxima
	;
	descartaArvoresNaoIsoladas(TodasTendas, AsOutrasArvores, ArvoresIsoladas),!.

%==============================================



/*===========================================
Predicado auxiliar vizinhancasLivresArvores/2

vizinhancasLivresArvores/2 e um predicado auxiliar que calcula as vizinhancas livres
(i.e, celulas da vizinhanca sem objectos) de cada arvore quando recebe uma lista de celulas com arvores

Usado como auxiliar em:
unicaHipotese/1

Caso terminal quando ja nao ha mais arvores a analisar */
vizinhancasLivresArvores(_, []):-!.

vizinhancasLivresArvores(Tabuleiro, [Arvore|AsOutrasArvores]):-
	vizinhanca(Arvore, Vizinhanca),
	
	%Das celulas da vizinhanca, seleciono aquelas que estao livres
	findall((Lin,Col),
	(member((Lin,Col),Vizinhanca),	
	celulaLivre(Tabuleiro, (Lin,Col))),
	VizinhancaLivre),
	
	length(VizinhancaLivre, NumCelsLivres),
	%Chamo este predicado auxiliar para avaliar se ha apenas uma hipotese ou nao
	%e inserir uma tenda caso haja
	insereNaUnicaHipotese(Tabuleiro, VizinhancaLivre, NumCelsLivres),
	
	%Avanco para a proxima arvore
	vizinhancasLivresArvores(Tabuleiro, AsOutrasArvores).

%============================================



/*========================================
Predicado Auxiliar insereNaUnicaHipotese/3

insereNaUnicaHipotese/3 e um predicado auxiliar que insere uma tenda na unica hipotese
se a vizinhanca livre duma arvore apenas tiver uma celula

Usado como auxiliar em:
vizinhancasLivresArvores/2 (predicado auxiliar) */

insereNaUnicaHipotese(Tabuleiro, VizinhancaLivre, NumCelsLivres):-
	NumCelsLivres == 1,
	
	nth1(1, VizinhancaLivre, CelulaLivre),
	insereObjectoCelula(Tabuleiro, t, CelulaLivre)
	;
	%Se houver mais do que uma celula livre, nao se faz nada e segue-se em frente
	NumCelsLivres \== 1, !.
	
%=========================================



/*=======================
Predicado unicaHipotese/1

Usado como auxiliar em:
estrategiasPrincipais (predicado auxiliar) */

unicaHipotese( (Tabuleiro, _, _)):-
	todasCelulas(Tabuleiro, TodasTendas, t),
	todasCelulas(Tabuleiro, TodasArvores, a),
	
	%Filtro a lista TodasArvores
	descartaArvoresNaoIsoladas(TodasTendas, TodasArvores, ArvoresIsoladas),

	%Depois da lista filtrada, quero saber as vizinhancas livres de cada arvore e
	%inserir uma tenda na unica hipotese se for o caso
	vizinhancasLivresArvores(Tabuleiro, ArvoresIsoladas).


%========================



/*================
Predicado valida/2

Usado como auxiliar em:
resolvido/1 (predicado auxiliar)

Para associar uma arvore a uma e so uma tenda e necessario que: */
valida(LArvores, LTendas):-
	%Nao existam arvores isoladas (i.e, sem pelo menos uma tenda na vizinhanca),
	descartaArvoresNaoIsoladas(LTendas, LArvores, ArvoresIsoladas),
	length(ArvoresIsoladas, NumIsoladas),
	NumIsoladas == 0,
	
	%O numero de arvores seja igual ao numero de tendas 
	length(LArvores, NumArvores),
	length(LTendas, NumTendas),
	NumArvores == NumTendas.

%=================



/*============================
Predicado Auxiliar resolvido/1

Este predicado auxiliar recebe um puzzle e verifica se esta resolvido ou nao

Usado como auxiliar em:
resolve/2 (predicado auxiliar)
resolveTentativa/3 (predicado auxiliar)

Um puzzle esta resolvido quando: */
resolvido( (Tabuleiro, TendasSupostasLinhas, TendasSupostasColunas) ):-
	%Nao ha celulas livres (estao todas preenchidas)
	todasCelulas(Tabuleiro, CelulasLivres, _),
	length(CelulasLivres, NumLivres),
	NumLivres == 0,
	
	%As listas que dizem o numero de tendas que devem existir por linha e por coluna
	%sao iguais as que dizem o numero de tendas que realmente existem
	calculaObjectosTabuleiro(Tabuleiro, TendasExistentesLinhas, TendasExistentesColunas, t),
	TendasSupostasLinhas == TendasExistentesLinhas,
	TendasSupostasColunas == TendasExistentesColunas,
	
	%E possivel associar uma tenda a uma e uma so arvore
	todasCelulas(Tabuleiro, TodasArvores, a),
	todasCelulas(Tabuleiro, TodasTendas, t),
	valida(TodasArvores, TodasTendas),

	%Nao ha tendas nas vizinhancas alargadas de outras
	todasVizinhancasTendas(Tabuleiro, TodasVizinhancasTendas),
	%TendasErradas e a lista de tendas que estao numa celula
	%que pertence a vizinhanca alargada doutra tenda
	findall(Cel, (member(Cel, TodasVizinhancasTendas), member(Cel, TodasTendas)), TendasErradas),
	%E esta lista deve estar vazia para o puzzle estar bem resolvido
	length(TendasErradas, Erros),
	Erros == 0.

%=============================



/*========================================
Predicado Auxiliar estrategiasPrincipais/1

Este predicado aplica 4 das 5 estrategias definidas para resolver o tabuleiro

Usado como auxiliar em:
resolve/2 (predicado auxiliar)
resolveTentativa/3 (predicado auxiliar) */


estrategiasPrincipais(Puzzle):-
	%Para preencher ou linhas e colunas que e suposto terem 0 tendas
	%ou aquelas que ja tem todas as tendas que e suposto terem
	relva(Puzzle),

	aproveita(Puzzle),
	%Acabamos de colocar uma tenda, logo e preciso limpar-lhe a vizinhanca
	limpaVizinhancas(Puzzle),
	%Como ela pode ter sido a ultima tenda que era preciso colocar
	%numa linha ou coluna aplica-se o relva/1
	relva(Puzzle),

	unicaHipotese(Puzzle),
	%Mais uma vez, como acabamos de por uma tenda limpamos-lhe a vizinhanca
	%e verificamos se podemos por mais relva 
	limpaVizinhancas(Puzzle),
	relva(Puzzle).

/*
	Este ultimo relva/1 poderia nao estar aqui uma vez que este predicado
	estrategiasPrincipais/1 vai ser repetido.
	Contudo, no resolveTentativa/3, quando e repetido, tem mais uma tenda aleatoria colocada
	por isso e melhor tentar resolver o puzzle ao maximo antes de colocar outra tenda aleatoria

========================================*/



/*=========================================================================
Predicado resolve/1 e Predicados Auxiliares resolve/2 e resolveTentativa/3

Como este e o problema mais complexo, segue-se uma explicacao detalhada do raciocinio
Assim os predicados em si nao estarao tao obstruidos por comentarios


Explicacao do raciocinio:
Puzzle entra no resolve/1 que chama resolve/2 criando uma lista vazia
resolve/2 fica a executar o 3o caso em que aplica as varias estrategias ate uma de duas coisas acontecerem:

o puzzle esta resolvido (programa acaba);

os dois ultimos elementos a serem adicionados a lista das versoes anteriores do puzzle
(as duas ultimas versoes) sao iguais, logo o puzzle nao esta a evoluir.
Nesse caso chama-se o resolveTentativa/3


o resolveTentativa/3 recebe o puzzle atual e um puzzle para poder recuperar caso algo corra mal,
e tambem uma lista para as versoes do puzzle. Quando e chamado pelo resolve/2,
o resolveTentativa/3 tem o puzzle principal igual ao puzzle de recuperacao


o resolveTentativa/3 fica a executar o 3o caso em que tenta resolver o puzzle colocando uma
tenda aleatoria e aplicando as estrategias principais ate uma de duas coisas acontecerem:

o puzzle esta resolvido (programa acaba);

os dois ultimos elementos a serem adicionados a lista das versoes anteriores do puzzle
(as duas ultimas versoes) sao iguais, logo o puzzle nao esta a evoluir.
Tambem e possivel que nao esteja a evoluir porque esta preenchido na totalidade,
mas a solucao nao esta bem 

Neste caso de ja nao evoluir mais chama-se o resolveTentativa/3 de novo, mas
com o puzzle de recuperacao como puzzle principal,
ou seja, voltamos ao puzzle antes de aplicar a tentativa */



resolve(Puzzle):-resolve(Puzzle, []).


%Puzzle esta resolvido
resolve(Puzzle, _):-
	resolvido(Puzzle), !.

%Puzzle ja nao evolui mais
resolve(Puzzle, [UltimaVersao, PenultimaVersao|_]):-
	UltimaVersao == PenultimaVersao,
	resolveTentativa(Puzzle, Puzzle, _).

%Tenta-se resolver o puzzle
resolve(Puzzle, VersoesAnteriores):-
	Puzzle = (Tabuleiro, _, _),
	inacessiveis(Tabuleiro),
	
	estrategiasPrincipais(Puzzle),	

	resolve(Puzzle, [Puzzle|VersoesAnteriores]).



%Puzzle esta resolvido
resolveTentativa(Puzzle, _, _):-
	resolvido(Puzzle), !.

%Puzzle ja nao evolui mais
resolveTentativa(_, PuzzleRecuperacao, [UltimaVersao, PenultimaVersao|_]):-
	UltimaVersao == PenultimaVersao,
	resolveTentativa(PuzzleRecuperacao, PuzzleRecuperacao, _).

%Tenta-se resolver o puzzle
resolveTentativa(Puzzle, PuzzleRecuperacao, VersoesAnterioresTentativa):-
	Puzzle = (Tabuleiro, _, _),
	
	todasCelulas(Tabuleiro, CelulasLivres, _),
	random_member(RandCel, CelulasLivres),
	insereObjectoCelula(Tabuleiro, t, RandCel),

	limpaVizinhancas(Puzzle),
	
	%Ja nao aplicamos o inacessiveis/1 pois e desnecessario, as celulas inacessiveis sao sempre as mesmas
	%Apenas se aplicam as estrategias principais
	estrategiasPrincipais(Puzzle),

	resolveTentativa(Puzzle, PuzzleRecuperacao, [Puzzle|VersoesAnterioresTentativa]).

%==========================================================================
















