'''
LEIC-A Ano letivo 2023-2024
Projeto 2 Fundamentos de Programação
Alexandre Carapeto Delgado ist1109441

Este projeto define 3 tipos abstratos de dados (TADs) e várias funções de alto
nível que fazem uso deles para emular um jogo de "Go" puramente à base de texto
'''


'''
Funções auxiliares #1 e #2

Funções auxiliares para converter números para as letras das coordenadas
das interseções e vice-versa: letras de A a Z maiúsculas correspondem
ao intervalo [65,99] na tabela do código ASCII

ao subtrair 64 passamos a trabalhar no intervalo [1,26],
onde cada letra corresponde à sua posição no alfabeto
ao adicionar 64 passamos do intervalo [1,26] para o [65,99]
e conseguimos converter para a letra que queremos
'''
#letra_p_num : string --> inteiro
def letra_p_num(letra):
    return ord(letra)-64

#num_p_letra : inteiro --> string
def num_p_letra(num):
    return chr(num+64)



# ==== TAD interseção ====

# -- Operações básicas --


#Construtor
'''
Cria o tipo abstrato de dados "interseção"
A representação interna escolhida foi a de um tuplo
cujo primeiro elemento é a letra da coluna da interseção
e o segundo é o número da linha

A coluna tem que:
ser uma string; ter comprimento 1; letra tem de ser de A a S (inclusive)
(S é a 19ª letra)
A linha tem que:
ter um inteiro; pertencer ao intervalo [1;19]
(o tabuleiro 19x19 é o maior tabuleiro de Go que existe)

Como em python o tipo booleano é uma subclasse do tipo int, se passarmos um
valor lógico para uma função isinstance() que esteja à espera dum inteiro, não
irá devolver False. Por isso usei a função type() com a qual isto não acontece
pois devolve o tipo do dado
'''
#cria_interseção : string x inteiro --> intersecao
def cria_intersecao(coluna, linha):
    if not isinstance(coluna, str) or len(coluna) != 1\
    or letra_p_num(coluna) not in range(1,20)\
    or type(linha) != int or linha not in range(1,20):        
        
        raise ValueError("cria_intersecao: argumentos invalidos")


    return (coluna, linha)



#Seletores
'''
Acedem aos índices do tuplo que representa a interseção
para ir buscar as coordenadas
'''
#obtem_col : intersecao --> string
def obtem_col(inters):
    return inters[0]

#obtem_lin : intersecao --> inteiro
def obtem_lin(inters):
    return inters[1]


'''
Função auxiliar #3 (é um seletor)
Foi criada para ao longo do código não repetir sempre
a conversão da letra da coluna para o número correspondente

Provavelmente será necessário ajustar o número para os índices fazendo -1
'''
#obtem_num_col : intersecao --> inteiro
def obtem_num_col(inters):
    return letra_p_num(obtem_col(inters))



#Reconhecedor
'''
Uma interseção, de acordo com a minha representação interna, tem que:
ser um tuplo; ter uma string de comprimento 1 como coordenada da coluna;
ter uma letra entre A e S (inclusive) como letra da coluna (S é a 19ª letra);
ter um inteiro no intervalo [1;19] como coordenada da linha 
'''
#eh_interseção : universal --> booleano
def eh_intersecao(candidato):
    return isinstance(candidato, tuple)\
    \
    and isinstance(obtem_col(candidato), str)\
    and len(obtem_col(candidato)) == 1\
    and obtem_num_col(candidato) in range(1,20)\
    \
    and isinstance(obtem_lin(candidato), int)\
    and obtem_lin(candidato) in range(1,20)



#Teste
'''
Duas interseções são iguais se ambas as coordenadas forem iguais
'''
#intersecoes_iguais : universal  x universal --> booleano
def intersecoes_iguais(candidato1, candidato2):
    return candidato1[0] == candidato2[0] and candidato1[1] == candidato2[1]



#Transformadores
'''
Convertem a interseção na minha representação interna
para a representação externa em string e vice-versa
'''
#intersecao_para_str : intersecao --> string
def intersecao_para_str(inters):
    return inters[0] + str(inters[1])

#str_para_intersecao : string --> intersecao
def str_para_intersecao(inters_str):
    return cria_intersecao(inters_str[0], int(inters_str[1:]))




# -- Funções de alto nível --

''''
Cada interseção tem, no máximo, 4 interseções adjacentes:
2 são na mesma coluna, andando uma linha para cima (+1) e outra para baixo (-1)

2 são na mesma linha, andando uma coluna
para a esquerda (+1) e outra para a direita (-1)

Contudo, se a interseção dada estiver numa borda,
então terá menos que 4 interseções adjacentes.
Como as coordenadas da última interseção nos vão dar o tamanho do goban,
sabemos que a interseção dada pode estar:

na linha mais em baixo (número é 1);
na linha mais em cima (número é a linha da última interseção do goban);
na coluna mais à esquerda (letra é A);
na coluna mais à direita (número da letra é o número da letra da última
interseção, que é igual ao número da sua linha pois o goban é um quadrado)
'''
#obtem_intersecoes_adjacentes : intersecao x intersecao --> tuplo
def obtem_intersecoes_adjacentes(inters, ultima_inters):
    inters_adj = []

    if obtem_lin(inters) != 1:
        inters_adj += [cria_intersecao(obtem_col(inters), obtem_lin(inters)-1)]
    
    if obtem_num_col(inters) != 1:
        inters_adj += [cria_intersecao(num_p_letra(obtem_num_col(inters)-1),\
                    obtem_lin(inters))]
    
    if obtem_num_col(inters) != obtem_lin(ultima_inters):
        inters_adj += [cria_intersecao(num_p_letra(obtem_num_col(inters)+1),\
                    obtem_lin(inters))]
    
    if obtem_lin(inters) != obtem_lin(ultima_inters):
        inters_adj += [cria_intersecao(obtem_col(inters), obtem_lin(inters)+1)]

    return tuple(inters_adj)



'''
A ordem de leitura é da esquerda para a direita seguida de de cima para baixo

Função lambda faz a função sort ordenar a lista primeiro pelas linhas
e só depois pelas colunas. Assim, geram-se aglomerados de interseções com
a mesma linha, ordenados por ordem crescente, e as interseções
dentro dos aglomerados são depois ordenadas pelas colunas 
'''
#ordena_intersecoes : tuplo --> tuplo
def ordena_intersecoes(tup_inters):
    l_inters = list(tup_inters)

    l_inters.sort(key = lambda x: (x[1], x[0]))
    return tuple(l_inters)




# ==== TAD pedra ====

# -- Operações básicas --


#Construtores
'''
Criam o tipo abstrato de dados "pedra"
A representação interna escolhida foi a de uma string
que contém O para pedras brancas, X para pedras pretas e . para pedras neutras
'''
#cria_pedra_branca : {} --> pedra
def cria_pedra_branca():
    return "O"

#cria_pedra_preta : {} --> pedra
def cria_pedra_preta():
    return "X"

#cria_pedra_neutra : {} --> pedra
def cria_pedra_neutra():
    return "."



#Reconhecedores
'''
Um valor é uma pedra se for igual àquilo que as funções construtoras devolvem
'''
#eh_pedra : universal --> booleano
def eh_pedra(candidato):
    return candidato in [cria_pedra_branca(), cria_pedra_preta(),\
                        cria_pedra_neutra()]

#eh_pedra_branca : pedra --> booleano
def eh_pedra_branca(pedra):
    return pedra == cria_pedra_branca()

#eh_pedra_preta : pedra --> booleano
def eh_pedra_preta(pedra):
    return pedra == cria_pedra_preta()



#Teste
'''
Para dois valores serem pedras iguais têm ambos que
ser pedras e ambos que ser iguais
'''
#pedras_iguais : universal x universal --> booleano
def pedras_iguais(candidato1, candidato2):
    return eh_pedra(candidato1) and eh_pedra(candidato2)\
    and candidato1 == candidato2



#Transformador
'''
Como a representação interna escolhida para a pedra foi string, esta função
apenas precisa de dar return da pedra como ela é
'''
#pedra_para_str : pedra --> string
def pedra_para_str(pedra):
    return pedra




# -- Funções alto nível --
'''
Uma pedra é dum jogador se for branca ou preta. Se não for
nem branca nem preta, é uma pedra neutra, ou seja, uma interseção livre
'''
#eh_pedra_jogador : pedra --> booleano
def eh_pedra_jogador(pedra):
    return eh_pedra_branca(pedra) or eh_pedra_preta(pedra)




# ==== TAD goban ====

# -- Operações básicas --


#Construtores
'''
Criam o tipo abstrato de dados "goban".
A representação interna escolhida para o goban foi
uma lista cujos elementos são listas.
Cada sublista representa uma coluna do tabuleiro.

Foi escolhida esta representação devido à familiaridade com a estrutura
devido ao 1.º Projeto, mas mudou-se de tuplo para lista para ser
mais fácil manipular a estrutura, já que tuplos são imutáveis
'''
#cria_goban_vazio: inteiro --> goban
def cria_goban_vazio(n):
    if not isinstance(n, int) or n not in [9, 13, 19]:
        raise ValueError("cria_goban_vazio: argumento invalido")

    return list( list( cria_pedra_neutra() for j in range(n) )\
                for i in range(n) )



'''
Para podermos criar um goban com a dimensão e as interseções fornecidas:
> A dimensão n tem que ser um inteiro e ser 9, 13 ou 19;
> Os tipos de dados que contêm as interseções têm de ser tuplos;

> Não podem haver interseções repetidas dentro dum mesmo tuplo;
Como o método set() gera um conjunto de elementos únicos, se algum dos tuplos
tiver elementos repetidos, o seu comprimento será diferente ao do seu set

Assim, a condição not len(set(inters_b)) == len(inters_b) irá retornar True se
houver interseções repetidas no tuplo das interseções brancas
(é feito o mesmo para o tuplo de interseções pretas)

> Todos os elementos dos tuplos têm que ser interseções;

> Os dois tuplos não podem ter a mesma interseção;
Verificar que um elemento de inters_b não está em inters_p é o mesmo que
fazer o contrário. Logo, apenas precisamos de fazer essa verificação uma vez

> As interseções fornecidas têm que estar dentro do goban.
Logo, n tem que ser maior ou igual tanto ao número da coluna da interseção
como à linha da interseção. Apenas se verifica se as coordenadas são maiores
do que os limites do goban porque já verificámos que o elemento é
uma interseção logo não terá a coordenada da linha abaixo de 1 por exemplo
'''
#cria_goban : inteiro x tuplo x tuplo --> goban
def cria_goban(n, inters_b, inters_p):
    if not isinstance(n, int) or n not in [9, 13, 19]\
    or not isinstance(inters_b, tuple) or not isinstance(inters_p, tuple)\
    or not len(set(inters_b)) == len(inters_b)\
    or not len(set(inters_p)) == len(inters_p):
        raise ValueError("cria_goban: argumentos invalidos")

    for i in inters_b:
        if not eh_intersecao(i) or i in inters_p\
        or obtem_num_col(i) > n or obtem_lin(i) > n:
            raise ValueError("cria_goban: argumentos invalidos")
    
    for i in inters_p:
        if not eh_intersecao(i) or obtem_num_col(i) > n or obtem_lin(i) > n:
            raise ValueError("cria_goban: argumentos invalidos")


    #Cria-se um goban vazio e "enche-se" o goban com as pedras
    goban = cria_goban_vazio(n)

    #Faz-se -1 para ajustar as coordenadas aos índices (pois estes partem do 0)
    for i in inters_b:
       goban[obtem_num_col(i)-1][obtem_lin(i)-1] = cria_pedra_branca()

    for i in inters_p:
        goban[obtem_num_col(i)-1][obtem_lin(i)-1] = cria_pedra_preta()

    return goban



'''
Esta função efetua uma deep copy do goban
Para cada sublista do goban, adicionamos uma sublista vazia à cópia e
adicionamos cada pedra à cópia uma a uma
'''
#cria_copia_goban : goban --> goban
def cria_copia_goban(goban):
    copia_goban = []

    for i in range(len(goban)):
        copia_goban.append([])
        
        for j in goban[i]:
            copia_goban[i] += [j]
    
    
    return copia_goban



#Seletores
'''
A última interseção é a interseção do canto superior direito do goban
Se o goban tiver um comprimento de 9 então sabemos que a sua última interseção
é a I9 pois o goban é um quadrado.

Se tiver comprimento de 13 então a última interseção é a M13

Se não tiver comprimento nem 9 nem 13, como apenas pode ter comprimentos
9, 13 e 19, então a última interseção vai ser a S19
'''
#obtem_ultima_intersecao : goban --> intersecao
def obtem_ultima_intersecao(goban):
    if len(goban) == 9:
        return cria_intersecao("I", 9)
    
    elif len(goban) == 13:
        return cria_intersecao("M", 13)
    
    else:
        return cria_intersecao("S", 19)



'''
Acedo a cada pedra ajustando as coordenadas da interseção aos índices
das listas e sublistas do goban
Se a pedra não for branca nem preta, então é neutra

Nota: apesar de ser uma função de operações básicas do TAD goban e podermos
usar a representação interna do goban, não podemos usar a representação interna
da pedra (estaríamos a quebrar abstração), daí ser necessário usar os
reconhecedores eh_pedra_branca() e eh_pedra_preta()
'''
#obtem_pedra : goban x intersecao --> pedra
def obtem_pedra(goban, inters):
    if eh_pedra_branca(goban[obtem_num_col(inters)-1][obtem_lin(inters)-1]):
        return cria_pedra_branca()
    
    elif eh_pedra_preta(goban[obtem_num_col(inters)-1][obtem_lin(inters)-1]):
        return cria_pedra_preta()
    
    else:
        return cria_pedra_neutra()



'''
Função auxiliar #4 - é uma operação básica do TAD goban
Esta função serve para ajudar na função obtem_cadeia()

Esta função recebe um goban e uma interseção e devolve uma lista
com as interseções adjacentes que lhe são iguais

Ou seja, se a interseção dada tiver uma pedra branca, a função irá devolver
apenas as interseções adjacentes que contenham pedras brancas
e o mesmo acontece para as pedras pretas e neutras
'''
#obtem_adjacentes_iguais : goban x intersecao --> lista
def obtem_adjacentes_iguais(goban, inters):
    adjacentes_iguais = []

    for i in obtem_intersecoes_adjacentes(inters,\
                                          obtem_ultima_intersecao(goban)):
        
        if pedras_iguais(obtem_pedra(goban, inters), obtem_pedra(goban, i)):
            adjacentes_iguais += [i]


    return adjacentes_iguais



'''
Começo por iterar pela lista das interseções do mesmo tipo que a dada
Se a interseção já estiver registada na cadeia,
simplesmente removo-a e passo para a seguinte na lista
Se ainda não estiver registada na cadeia, registo-a.

Depois, vou buscar as interseções adjacentes a essa que lhe são iguais
e adiciono-as à lista de interseções por analisar.
Por fim, removo a interseção dessa lista.


Essencialmente, tenho uma lista de interseções que quero analisar.
Essa lista começa com as interseções adjacentes iguais à interseção inicial.
À medida que vou analisando as interseções, adiciono-as à cadeia
se ainda não estiverem lá e vou buscar mais interseções iguais
que são adjacentes a essas.

Vou também removendo interseções que já analisei
para não ficar preso num ciclo infinito. Quando a lista das interseções
a analisar ficar vazia, o ciclo while pára e tenho a cadeia completa
'''
#obtem_cadeia : goban x intersecao --> tuplo
def obtem_cadeia(goban, inters):
    cadeia = [inters]
    inters_a_analisar = obtem_adjacentes_iguais(goban, inters)

    while inters_a_analisar != []:
        for i in inters_a_analisar:

            if i not in cadeia:
                cadeia += [i]
                inters_a_analisar += obtem_adjacentes_iguais(goban, i)

            inters_a_analisar.remove(i)


    return ordena_intersecoes(tuple(cadeia))



#Modificadores
'''
Muda destrutivamente a interseção dada para ter a pedra fornecida
'''
#coloca_pedra : goban x intersecao x pedra --> goban
def coloca_pedra(goban, inters, pedra):
    goban[obtem_num_col(inters)-1][obtem_lin(inters)-1] = pedra
    return goban

'''
Muda destrutivamente a interseção dada para ter uma pedra neutra
(retira a pedra que estava lá anteriormente)
'''
#remove_pedra : goban x intersecao --> goban
def remove_pedra(goban, inters):
    goban[obtem_num_col(inters)-1][obtem_lin(inters)-1] = cria_pedra_neutra()
    return goban

'''
Muda destrutivamente todas as interseções pertencentes à cadeia dada para
pedras neutras (retira as pedras que estavam lá anteriormente)
'''
#remove_cadeia : goban x tuplo --> goban
def remove_cadeia(goban, cadeia):
    for i in cadeia:
        goban = remove_pedra(goban, i)
    return goban



#Reconhecedores
'''
De acordo com a minha representação interna, um goban tem que:
ser uma lista de comprimento pertencente ao intervalo [1;19];
ter sublistas todas do mesmo comprimento como seus elementos;
ter uma pedra em cada elemento de cada sublista
'''
#eh_goban : universal --> booleano
def eh_goban(candidato):

    if not isinstance(candidato, list) or len(candidato) not in range(1,20):
        return False

    for i in candidato:         
        if not isinstance(i, list):
            return False


    for i in range(len(candidato)-1,-1,-1):
        
        if len(candidato[i]) != len(candidato[i-1])\
        or len(candidato[i]) != len(candidato):
            return False
        
        for j in candidato[i]:
            if not eh_pedra(j):
                return False
    

    #é desnecessário ter um else statement pois a função termina e devolve
    #False sempre que conclui que o candidato não é um goban
    return True



'''
Uma interseção pertence ao goban (é válida) se nenhuma das suas coordenadas
for maior que a sua dimensão
len(goban) é a dimensão n do goban, goban é n x n
'''
#eh_intersecao_valida : goban x intersecao --> booleano
def eh_intersecao_valida(goban, inters):
    return len(goban) >= obtem_num_col(inters)\
    and len(goban) >= obtem_lin(inters)



#Teste
'''
Primeiro, é preciso verificar que ambos os argumentos são gobans
Depois, para concluir que dois gobans são iguais sem quebrar nenhuma
barreira de abstração, é preciso verificar que:

ambos têm o mesmo comprimento;
as sublistas dos dois têm todas o mesmo comprimento;
duas pedras correspondentes à mesma coordenada nos dois gobans são iguais
'''
#gobans_iguais : universal x universal --> booleano
def gobans_iguais(candidato1, candidato2):
    
    if not eh_goban(candidato1) or not eh_goban(candidato2)\
    or len(candidato1) != len(candidato2):
        return False


    for i in range(len(candidato1)):
        
        if len(candidato1[i]) != len(candidato2[i]):
            return False

        for j in range(len(candidato1)):
            if not pedras_iguais(candidato1[i][j], candidato2[i][j]):
                return False


    return True



#Transformador
'''
Numa range crescente até ao comprimento do goban, vou convertendo o número
em letra e adiciono à string para criar a primeira linha com as letras


Para criar cada linha, que contém um número no início e no fim e cada pedra,
uso dois loops for:
O loop for exterior itera numa range decrescente pois os números das linhas
estão ordenados por ordem descrescente (o maior está em cima)
O loop interior itera simplesmente numa range crescente até ao comprimento da
lista goban (começando em 1 e incluindo o comprimento do goban)

Assim, a variável j vai-me dando o índice de cada coluna para cada i e
a variável i dá-me o número da linha a que estou a aceder e consigo adicionar
o número da linha à string e, no loop interior, criar a interseção em que estou

Obtenho a pedra e converto-a para string e adiciono-a à string do goban


Por fim, adiciono de novo a linha de letras à string
'''
#goban_para_str : goban --> string
def goban_para_str(goban):
    goban_str = "  "

    for i in range(len(goban)):
        goban_str += " " + num_p_letra(i+1)
    linha_letras = goban_str
    
    goban_str += "\n"


    #range a começar na dimensão do goban e a acabar em 1 (inclusive)
    for i in range(len(goban),0,-1):
       
        if len(goban) > 9 and i > 9:
            goban_str += str(i) + " "
        else:
            goban_str += " " + str(i) + " "


        #interseção pela qual estou a passar tem coluna j e linha i (range do j
        #está ajustada para começar em 1 e acabar na len do goban (inclusive))
        for j in range(1, len(goban)+1):
            inters = cria_intersecao(num_p_letra(j),i)
            goban_str += pedra_para_str(obtem_pedra(goban, inters)) + " "

        
        if len(goban) > 9 and i > 9:
            goban_str += str(i) + "\n"
        else:
            goban_str += " " + str(i) + "\n"


    goban_str += linha_letras
    return goban_str




# -- Funções de alto nível --

'''
Faço dois loops, ambos numa range crescente até à dimensão n do goban.
Com o loop exterior acompanho a coordenada da linha
e com o interior acompanho a coordenada da coluna.

Como estou a analisar as interseções do goban uma a uma
pela ordem de leitura, o território irá ser criado já ordenado pela ordem de
leitura da primeira interseção. Como a função obtem_cadeia() ordena as
interseções pela ordem de leitura, cada território também já estará ordenado.

Para cada interseção do goban que não tenha nenhuma pedra
(ou seja, que tenha uma pedra neutra) obtenho a cadeia a que ela pertence
Adiciono essa cadeia à lista dos territórios


Para não fazer verificações desnecessárias nem repetir cadeias, por cada
interseção na cadeia que acabei de ir buscar coloco uma pedra dum jogador

Assim, quando o loop efetuar de novo a verificação se a interseção é ou não
livre já não irá buscar uma cadeia que foi previamente calculada
'''
#obtem_territorios : goban --> tuplo
def obtem_territorios(goban):
    territ = []

    #n é a dimensão do goban
    n = obtem_lin(obtem_ultima_intersecao(goban)) 
    copia_goban = cria_copia_goban(goban)


    #range de 1 a n (inclusive) para os índices baterem certo com as
    #coordenadas quando criar as interseções 
    for i in range(1, n+1):
        for j in range(1, n+1):
            inters = cria_intersecao(num_p_letra(j), i)

            if not eh_pedra_branca(obtem_pedra(copia_goban, inters))\
            and not eh_pedra_preta(obtem_pedra(copia_goban, inters)):
                
                cadeia = obtem_cadeia(copia_goban, inters)
                territ += (cadeia,)

                for k in cadeia:
                    copia_goban = coloca_pedra(copia_goban, k,\
                                            cria_pedra_branca())


    return tuple(territ)



'''
Para cada interseção do tuplo de interseções vou buscar as
suas interseções adjacentes. Para cada interseção adjacente verifico se:
a interseção original é livre (tem uma pedra neutra) e a interseção adjacente
tem uma pedra (ou branca ou preta)
a interseção original tem uma pedra e a interseção adjacente é livre

Se uma destas condições for verificada, significa que as duas interseções
(a original e a adjacente) são diferentes
Adiciono à lista
'''
#obtem_adjacentes_diferentes : goban x tuplo --> tuplo
def obtem_adjacentes_diferentes(goban, t_inters):
    adj_dif = []

    for i in t_inters:
        for j in obtem_intersecoes_adjacentes(i,obtem_ultima_intersecao(goban)):

            '''
            pedra em i é neutra e pedra em j ou é branca ou preta
            ou pedra em i ou é branca ou preta e pedra em j é neutra
            (ser neutra é o mesmo que não ser nem branca nem preta)
            '''
            if ( not eh_pedra_jogador(obtem_pedra(goban, i))\
            and eh_pedra_jogador(obtem_pedra(goban, j)) )\
            or\
            ( eh_pedra_jogador(obtem_pedra(goban, i))\
            and not eh_pedra_jogador(obtem_pedra(goban, j)) ):
                
                if j not in adj_dif:
                    adj_dif += [j]


    return ordena_intersecoes(tuple(adj_dif))



'''
Primeiro, coloco a pedra pedida na interseção dada

Depois, para cada pedra adjacente a essa que é do jogador oposto
(não é neutra nem é igual à pedra dada)
obtenho a cadeia dessa pedra
e vejo se a cadeia possui alguma interseção adjacente diferente

A função obtem_adjacentes_diferentes() não diferencia entre
pedras pretas e brancas, por isso estou a ver se a cadeia tem alguma
interseção livre adjacente (alguma liberdade)
Se não tiver então devo remover a cadeia do goban pois foi eliminada
'''
#jogada : goban x intersecao x pedra --> goban
def jogada(goban, inters, pedra):
    goban = coloca_pedra(goban, inters, pedra)

    for i in obtem_intersecoes_adjacentes(inters,\
                                        obtem_ultima_intersecao(goban)):
        
        if ( eh_pedra_branca(pedra) and eh_pedra_preta(obtem_pedra(goban,i)) )\
        or ( eh_pedra_preta(pedra) and eh_pedra_branca(obtem_pedra(goban,i)) ):
            
            cadeia = obtem_cadeia(goban, i)
            if obtem_adjacentes_diferentes(goban, cadeia) == ():
                goban = remove_cadeia(goban, cadeia)


    return goban



'''
Tal como na função obtem_territorios() itero duas vezes
numa range crescente até à dimensão n do goban
De novo, acompanho a coordenada da linha com o loop exterior
e a da coluna com o loop interior

Percorro todas as interseções do goban e se a interseção que estou a analisar
tem uma pedra branca, aumento o número de peças brancas
se tem uma pedra preta, aumento o número de peças pretas
'''
#obtem_pedras_jogadores : goban --> tuplo
def obtem_pedras_jogadores(goban):
    num_brancas = 0
    num_pretas = 0

    #n é a dimensão do goban
    n = obtem_lin(obtem_ultima_intersecao(goban))


    for i in range(1, n+1):
        for j in range(1, n+1):
            inters = cria_intersecao(num_p_letra(j),i)
            
            if eh_pedra_branca(obtem_pedra(goban, inters)):
               num_brancas += 1
            if eh_pedra_preta(obtem_pedra(goban, inters)):
                num_pretas += 1


    return (num_brancas, num_pretas)




# ==== Funções adicionais ====
# (funções para o jogo em si)

'''
Os pontos de um jogador são o número de pedras que tem no tabuleiro mais
o comprimento dos territórios que são seus. 

Para cada território do goban quero ver se a sua fronteira é
exclusivamente composta por pedras pretas ou brancas
Para cada interseção adjacente diferente ao território vejo se
é uma pedra branca ou preta
Se for branca aumento o contador da fronteira branca
e se for preta aumento o das pretas


Se depois de analisar o território a fronteira preta continuar 0, então
a fronteira do território é toda branca e, logo, pertence ao jogador branco
Assim, aos pontos das brancas soma-se o número
de interseções que o território tem

Se a fronteira branca for 0, então os pontos do comprimento
do território são para as pretas
Se a fronteira for mista, então o território não é de ninguém
e não acrescenta pontos a nenhum jogador


Sempre que vou analisar um território novo
dou reset aos contadores das fronteiras

Para além disso, uso a função obtem_pedras_jogadores() para obter o número
de pedras de cada jogador e somar esses pontos aos pontos de cada jogador
'''
#calcula_pontos : goban --> tuplo
def calcula_pontos(goban):
    pts_brancas = 0
    pts_pretas = 0
    

    for i in obtem_territorios(goban):
        fronteira_b = 0
        fronteira_p = 0

        for j in obtem_adjacentes_diferentes(goban, i):
            if eh_pedra_branca(obtem_pedra(goban, j)):
                fronteira_b += 1
            if eh_pedra_preta(obtem_pedra(goban, j)):
                fronteira_p += 1

        '''
        também é preciso que a fronteira da cor à qual vou adicionar os pontos
        não seja 0 pois, assim, se o goban estiver vazio, ambos os jogadores
        vão ter a pontuação máxima em vez de 0
        '''
        if fronteira_p == 0 and fronteira_b > 0:
            pts_brancas += len(i)

        if fronteira_b == 0 and fronteira_p > 0:
            pts_pretas += len(i)


    pedras_jogadores = obtem_pedras_jogadores(goban)
    pts_brancas += pedras_jogadores[0]
    pts_pretas += pedras_jogadores[1]

    return (pts_brancas, pts_pretas)



'''
A jogada não é legal se:
tentarmos colocar uma pedra numa interseção que já tenha uma pedra
colocarmos uma pedra numa posição que causa suicídio
se a jogada resultar num tabuleiro igual ao tabuleiro dado para a regra do ko

Para verificar o suicídio, vejo se a cadeia à qual a interseção onde
estamos a jogar pertence (depois de jogarmos) tem liberdades
Se não tiver depois da jogada então é ilegal jogar aí
'''
#eh_jogada_legal : goban x intersecao x pedra x goban --> booleano
def eh_jogada_legal(goban, inters, pedra, goban_ko):
    copia_goban = cria_copia_goban(goban)

    if eh_pedra_branca(obtem_pedra(copia_goban, inters))\
    or eh_pedra_preta(obtem_pedra(copia_goban, inters)):
        return False


    copia_goban = jogada(copia_goban, inters, pedra)

    cadeia = obtem_cadeia(copia_goban, inters)
    if obtem_adjacentes_diferentes(copia_goban, cadeia) == ():
        return False

    if gobans_iguais(copia_goban, goban_ko):
        return False
    

    #é desnecessário ter a instrução else: pois a função acaba e devolve False
    #se entrar em qualquer uma das condições ifs
    return True



'''
Num ciclo infinito verifico, primeiro, se a variável inters é igual a "P"
e se for devolvo False pois o jogador passou
Depois, tento executar o código do bloco "try"
Este código pede infinitamente um input para a inters até a jogada ser legal
Quando for, uso a função jogada para jogar e devolvo True


Contudo, o input pode não ser uma interseção válida.

Como apenas uma interseção válida é capaz de passar na função
str_para_intersecao() sem levantar erros, quando verifico se a jogada é legal,
primeiro uso essa função para verificar se o input é uma interseção
e procuro erros específicos:

> ValueError acontece quando a interseção passada não é válida
(por exemplo se não estiver no goban ou for uma string inválida)
> IndexError acontece, por exemplo, quando o utilizador simplesmente
carrega no enter, passando uma string vazia
> TypeError acontece quando a função str_para_interseção() tenta ler
o valor "None" (é o primeiro caso e acontece sempre)

Se nenhum erro for levantado, então a interseção passada é válida e posso
verificar a legalidade da jogada e prosseguir
'''
#turno_jogador : goban x pedra x goban --> booleano
def turno_jogador(goban, pedra, goban_ko):
    #crio esta variável sem valor, só para já existir
    inters = None

    while True:
        if inters == "P":
            return False

        try:
            while not eh_jogada_legal(goban, str_para_intersecao(inters),\
                                    pedra, goban_ko):
                inters = input("Escreva uma intersecao ou 'P' para passar ["\
                            + pedra_para_str(pedra) + "]:")

            goban = jogada(goban, str_para_intersecao(inters), pedra)
            return True


        except (ValueError, IndexError, TypeError):
            inters = input("Escreva uma intersecao ou 'P' para passar ["\
                        + pedra_para_str(pedra) + "]:")



'''
Função principal que permite jogar o jogo ao utilizar tudo o que já definimos


A verificação dos dados de entrada é muito parecida à da função cria_goban().
A diferença é que como as interseções fornecidas estão na representação externa
preciso de convertê-las para interseções
Ao fazer essa conversão, posso encontrar uma série de erros e
nesse processo acabo por verificar que a interseção é válida

> ValueError acontece quando os elementos dos tuplos não são interseções
válidas na representação externa e por isso não podem ser convertidas
de string para interseção
> IndexError acontece quando os elementos dos tuplos são strings vazias
> TypeError acontece quando os elementos dos tuplos não sáo strings

Se pusesse a instrução raise no bloco except então ambos os erros
seriam mostrados, por isso uso uma variável auxiliar booleana para
dar raise posteriormente


Feita a verificação dos dados crio o goban do jogo
Preciso de criar uma variável turno brancas já, mas não uma preta, pois vou
usá-la numa condição a meio do loop e a turno pretas é criada no início do loop

A regra ko compara o goban atual com o goban de há dois turnos atrás
Preciso de um goban ko para cada jogador pois cada jogador tem um
goban diferente que não pode repetir quando joga


Depois da criação das variáveis iniciais, calculo os pontos dos jogadores
e imprimo essa informação mais o goban como string para o ecrã


Para o jogo em si tenho um loop principal que em cada iteração:

Pede às pretas para jogarem e atualiza o seu goban ko;
Atualiza e mostra a pontuação e o goban atual;
Verifica se ambos os jogadores passaram (variáveis dos turnos são ambas False);

Pede às brancas para jogarem e atualiza o seu goban ko;
Atualiza e mostra a pontuação e o goban atual;
Verifica se ambos os jogadores passaram (variáveis dos turnos são ambas False)


Quando as variáveis turno_brancas e turno_pretas forem as duas False
(ambos os jogadores passaram) o jogo termina

Com base na pontuação atual, devolve-se True se as brancas ganharem
'''
#go : inteiro x tuplo x tuplo --> booleano
def go(n, inters_b, inters_p):

    #Validação de argumentos
    argumentos_invalidos = False

    if not isinstance(n, int) or n not in [9, 13, 19]\
    or not isinstance(inters_b, tuple) or not isinstance(inters_p, tuple):
        raise ValueError("go: argumentos invalidos")
    

    try:
        inters_b = list(inters_b)
        for i in range(len(inters_b)):
            inters_b[i] = str_para_intersecao(inters_b[i])

        inters_p = list(inters_p)
        for i in range(len(inters_p)):
            inters_p[i] = str_para_intersecao(inters_p[i])
    except (ValueError, IndexError, TypeError):
        argumentos_invalidos = True

    if argumentos_invalidos:
        raise ValueError("go: argumentos invalidos")
   

    inters_b = tuple(inters_b)
    inters_p = tuple(inters_p)

    if not len(set(inters_b)) == len(inters_b)\
    or not len(set(inters_p)) == len(inters_p):
        raise ValueError("go: argumentos invalidos")

    for i in inters_b:
        if i in inters_p or obtem_num_col(i) > n or obtem_lin(i) > n:
            raise ValueError("go: argumentos invalidos")
        
    for i in inters_p:
        if obtem_num_col(i) > n or obtem_lin(i) > n:
            raise ValueError("go: argumentos invalidos")



    #Inicialização do jogo e variáveis
    goban = cria_goban(n, inters_b, inters_p)


    turno_brancas = True
    ko_pretas = None
    ko_brancas = cria_copia_goban(goban)
    
    #Mostrar pontos e goban iniciais
    pontos = calcula_pontos(goban)
    print("Branco (O) tem " + str(pontos[0]) + " pontos")
    print("Preto (X) tem " + str(pontos[1]) + " pontos")
        
    print(goban_para_str(goban))



    #Loop principal para utilizadores jogarem o jogo
    while True:
        #Função turno_jogador() já garante que a jogada é legal
        turno_pretas = turno_jogador(goban, cria_pedra_preta(), ko_pretas)
        ko_pretas = cria_copia_goban(goban)
        
        pontos = calcula_pontos(goban)
        print("Branco (O) tem " + str(pontos[0]) + " pontos")
        print("Preto (X) tem " + str(pontos[1]) + " pontos")
        
        print(goban_para_str(goban))
        

        if not turno_pretas and not turno_brancas:
            break
       
        

        turno_brancas = turno_jogador(goban, cria_pedra_branca(), ko_brancas)
        ko_brancas = cria_copia_goban(goban)
        
        pontos = calcula_pontos(goban)
        print("Branco (O) tem " + str(pontos[0]) + " pontos")
        print("Preto (X) tem " + str(pontos[1]) + " pontos")
        
        print(goban_para_str(goban))


        '''
        Quero verificar esta condição (a mesma do meio) no final de cada
        iteração, mas optei por não pôr uma equivalente como condição do while
        pois causa um timeout. Decidi usar um while True portanto
        '''
        if not turno_pretas and not turno_brancas:
            break




    #Brancas ganham se tiverem mais pontos ou se houver empate
    #(devolver True se vitória for das brancas)
    return pontos[0] >= pontos[1]



'''
FIM DO PROJETO
'''