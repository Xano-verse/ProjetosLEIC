'''
LEIC-A Ano letivo 2023-2024
Projeto 1 Fundamentos de Programação
Alexandre Carapeto Delgado ist1109441

Este projeto trabalha com estruturas retangulares com linhas verticais e
horizontais chamadas "territórios" e define várias funções que permitem
definir e estudar estas estruturas

Cada função contém uma explicação do código que está a executar
Nas funções que verificam a validade dos dados introduzidos,
de modo a não tornar a documentação excessiva, apenas incluí explicação para
aquelas com muitas condições
'''

'''
Funções auxiliares #1 e #2

Funções auxiliares para converter números para as letras das coordenadas
dos territórios e vice-versa: letras de A a Z maiúsculas correspondem
ao intervalo [65,99] na tabela do código ASCII

ao subtrair 64 passamos a trabalhar no intervalo [1,26],
onde cada letra corresponde à sua posição no alfabeto
ao adicionar 64 passamos do intervalo [1,26] para o [65,99]
e conseguimos converter para a letra que queremos
'''
def letra_p_num(letra):
    return ord(letra)-64

def num_p_letra(num):
    return chr(num+64)



'''
Situações a garantir que não acontecem:
Variável passada não é um tuplo;
esse tuplo está vazio ou excede os 26 elementos (só há 26 letras);

elementos do tuplo não são tuplos;
os vários subtuplos (linhas verticais) têm comprimentos diferentes;
subtuplos excedem os 99 elementos;

elementos dentro dos subtuplos não são 0s ou 1s;
elementos dentro dos subtuplos não são inteiros
(0.0 ou 1.0 não são valores permitidos)
'''
def eh_territorio(territ):
    #nunca são lançados erros pois a condição da len vem após
    #a que verifica que é tuplo
    if isinstance(territ, tuple) and len(territ) in range(1, 27):
        
        #antes de comparar os comprimentos dos vários elementos
        #tenho que verificar que TODOS os elementos são tuplos
        for i in range(len(territ)):

            if not isinstance(territ[i], tuple):
                return False

        #loop a decrementar para evitar index out of range
        for i in range(len(territ)-1, -1, -1):

            if len(territ[i]) != len(territ[i-1])\
            or not len(territ[i]) in range(1,100):
                return False

            for j in territ[i]:
                if (j != 0 and j != 1) or not isinstance(j, int):
                    return False

        #se não houver nenhum problema com o input
        return True

    #se territ nem for um tuplo
    return False



'''
A última interseção situa-se no canto superior direito
A linha vertical onde se encontra é a última
(o número dessa linha é o comprimento do tuplo)
Usa-se a função auxiliar num_p_letra para
se converter esse número na letra correspondente

A linha horizontal onde se encontra é a última
(o número dessa linha é o comprimento dos subtuplos)
'''
def obtem_ultima_intersecao(territ):
    return (num_p_letra(len(territ)), len(territ[0]))



'''
Para ser uma interseção válida é preciso:
ser um tuplo de 2 elementos;

o 1º elemento tem de ser uma string correspondente
a uma das letras maiúsculas;

o 2º elemento tem que ser um inteiro entre 1 e 99
'''
def eh_intersecao(inters):
    return isinstance(inters, tuple) and len(inters) == 2\
    \
    and isinstance(inters[0], str) and len(inters[0]) == 1\
    and letra_p_num(inters[0]) in range(1,27)\
    \
    and isinstance(inters[1], int) and inters[1] in range(1,100)



'''
Para a interseção pertencer ao território então:
a primeira coordenada tem que ser uma letra entre A
e a letra correspondente à última linha vertical do território
(o comprimento do tuplo territ) (inclusive)

a segunda coordenada tem que ser um número entre 1
e o comprimento dos subtuplos do tuplo territ (inclusive)
'''
def eh_intersecao_valida(territ, inters):
    return inters[0] in [num_p_letra(i) for i in range(1,len(territ)+1)]\
    and inters[1] in range(1, len(territ[0])+1)



'''
inters[0] é a letra (corresponde à linha vertical da interseção)
territ[letra_p_num(inters[0])-1] aponta para o tuplo que contém
os elementos dessa linha vertical (desconta-se 1 pois índices começam no 0)

inters[1] é o número (corresponde à linha horizontal da interseção)
o índice [inters[1]-1] (aplicado ao tuplo que contém a linha vertical desejada)
aponta para o elemento situado na linha horizontal do número
(desconta-se 1 pois índices começam no 0)

verifica-se se essa interseção tem valor 0 ou não (se é livre ou não)
'''
def eh_intersecao_livre(territ, inters):
    return territ[letra_p_num(inters[0])-1][inters[1]-1] == 0



'''
Cada interseção tem, no máximo, 4 interseções adjacentes:
2 são na mesma linha vertical (letra), andando uma linha horizontal
para cima (+1) e outra para baixo (-1)

2 são na mesma linha horizontal (número), andando uma linha vertical
para a esquerda (+1) e outra para a direita (-1)

Contudo, se a interseção dada estiver numa borda,
então terá menos que 4 interseções adjacentes. Interseção dada pode estar:
na linha horizontal mais em baixo (número é 1);
na linha horizontal mais em cima (número é o comprimento dos subtuplos);
na linha vertical mais à esquerda (letra é A);
na linha vertical mais à direita
(o número da letra é o comprimento do tuplo dado) 
'''
def obtem_intersecoes_adjacentes(territ, inters):
    inters_adj = []
    '''
    se a interseção não estiver na linha horizontal mais abaixo
    adiciona-se a interseção abaixo dessa
    - raciocínio semelhante feito nas outras condições
    '''
    if inters[1] != 1:
       inters_adj += [(inters[0], inters[1]-1)]

    #essencialmente é fazer if inters[0] != "A", manteve-se com a
    #função auxiliar de conversão por consistência com a próxima condição
    if letra_p_num(inters[0]) != 1:
        inters_adj += [(num_p_letra(letra_p_num(inters[0])-1), inters[1])]
    
    if letra_p_num(inters[0]) != len(territ):
        inters_adj += [(num_p_letra(letra_p_num(inters[0])+1), inters[1])]

    if inters[1] != len(territ[0]):
        inters_adj += [(inters[0], inters[1]+1)]
  
    return tuple(inters_adj)



'''
Sei que bastava fazer l_inters.sort(key = lambda x: (x[1], x[0])) para
ordenar tudo, primeiro pelo índice 1 e depois pelo índice 0
Contudo, quando fiz o código original não sabia usar o potencial máximo
da função lambda e, por isso, arranjei uma solução que
satisfez bem as necessidades do problema
Assim, decidi manter o código original, apesar de ser menos eficiente

É preciso ordenar segundo duas coisas, as letras e os números.
Ordeno primeiro tudo pelos números, gerando aglomerados de interseções
que estão ordenados pelos números.
Falta ordenar as interseções de cada aglomerado pela letra
'''
def ordena_intersecoes(t_inters):
    l_inters = list(t_inters)
    l_final = []
    
    #ordena as interseções pelos números, por ordem crescente
    l_inters.sort(key = lambda x: x[1])

    #x serve para saber até que índice já ordenei em relação à letra
    x = 0
    try:
        for i in range(len(l_inters)):
            '''
            sei que as interseções com o mesmo número estão todas juntas
            e que os aglomerados do mesmo número estão ordenados
            quando o número (índice 1 dos subtuplos) aumentar
            (mudei de aglomerado), então ordeno as interseções com
            esse número em relação à letra
            '''
            if l_inters[i][1] < l_inters[i+1][1]:
                l_final += sorted(l_inters[x:i+1])
                x = i+1

    #se atingimos o último número a slice irá acabar no índice igual à len
    except IndexError:
        l_final += sorted(l_inters[x:len(l_inters)])

    return tuple(l_final)



'''
O loop exterior garante que passo por todas as linhas horizontais do território
E para cada linha horizontal quero passar por todas as linhas verticais
e para isso uso o loop interior

Assim, o loop exterior itera numa range descrescente (pois quero começar
na linha horizontal mais acima que é a que tem o número maior)
que parte do comprimento dos subtuplos (têm todos os mesmo comprimento)
e pára em 0 (o 1 é o último valor tomado)

A variável i não serve para índices pois está 1 unidade adiantada,
por isso subtraio 1 se a quiser usar como índice

O loop interior itera sobre o território - assim, conseguimos aceder a
cada subtuplo. Como a variável i nos diz a linha horizontal onde estamos,
conseguimos aceder a cada interseção do território e
saber se está ocupada ou não
'''
def territorio_para_str(territ):
    if not eh_territorio(territ):
        raise ValueError("territorio_para_str: argumento invalido")
 
    territ_str = "  "
    dois_dig = False

    #gerar linha de letras
    for i in range(1, len(territ)+1):
        territ_str += " " + num_p_letra(i)

    #para não ter que repetir este loop    
    linha_letras = territ_str
    
    territ_str += "\n"
    
    '''
    no início, as coordenadas números têm um espaço antes
    no fim da linha, têm dois espaços
    no caso de haver coordenadas com 2 dígitos é preciso
    retirar um espaço tanto no número do início como no do fim para alinhar
    '''
    if len(territ[0]) > 9:
        dois_dig = True

    for i in range(len(territ[0]), 0, -1):
        
        #escrever número no início
        if dois_dig and i > 9:
            territ_str += str(i)
        else:
           territ_str += " " + str(i)
        
        #escrever . ou X
        for j in territ:
            if j[i-1] == 0:
                territ_str += " ."
            #se for 1
            else:
                territ_str += " X"
        
        #escrever número no fim
        if dois_dig and i > 9:
            territ_str += " " + str(i) + "\n"
        else:
            territ_str += "  " + str(i) + "\n"
    
    territ_str += linha_letras

    return territ_str



'''
Função auxiliar #3

Função auxiliar para ajudar na função obtem_cadeia
Esta função recebe um território e uma interseção e devolve
uma lista com as interseções adjacentes que lhe são iguais

Ou seja, se a interseção dada contiver uma montanha, a função irá
devolver apenas as interseções adjacentes que contenham montanhas
e se a interseção dada for livre, a função irá devolver
apenas as interseções adjacentes livres
'''
def obtem_adjacentes_iguais(territ, inters):
    adjacentes_iguais = []
    
    #das interseções adjacentes apenas seleciono
    #aquelas que são do mesmo tipo que a dada
    for i in obtem_intersecoes_adjacentes(territ, inters):

        #faz-se -1 para ajustar os índices pois
        #as coordenadas das interseções começam em 1 e não em 0
        if territ[letra_p_num(inters[0])-1][inters[1]-1] ==\
        territ[letra_p_num(i[0])-1][i[1]-1]:
            
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
def obtem_cadeia(territ, inters):
    if not eh_territorio(territ) or not eh_intersecao(inters)\
    or not eh_intersecao_valida(territ, inters):
        raise ValueError("obtem_cadeia: argumentos invalidos")
    

    cadeia = [inters]
    inters_a_analisar = obtem_adjacentes_iguais(territ, inters)

    while inters_a_analisar != []:
        for i in inters_a_analisar:

            if i not in cadeia:
                cadeia += [i]
                #vou buscar mais interseções adjacentes (só as iguais)
                inters_a_analisar += obtem_adjacentes_iguais(territ, i)
            
            #removo sempre a interseção em que estou
            inters_a_analisar.remove(i)

    return tuple(ordena_intersecoes(cadeia))



'''
Vou buscar a cadeia a que a interseção dada pertence.
Por cada interseção da cadeia vou buscar as suas interseções adjacentes
e verifico quais dessas estão livres e ainda não estão registadas no vale
As que passarem as condições são adicionadas à lista vale
'''
def obtem_vale(territ, inters):
    #para além das condições habituais, também quero verificar se
    #a interseção é livre - se é então não tem uma montanha
    if not eh_territorio(territ) or not eh_intersecao(inters)\
    or not eh_intersecao_valida(territ, inters)\
    or eh_intersecao_livre(territ, inters):
        raise ValueError("obtem_vale: argumentos invalidos")
    

    vale = []
    for i in obtem_cadeia(territ, inters):
        for j in obtem_intersecoes_adjacentes(territ, i):

            if eh_intersecao_livre(territ, j) and j not in vale:
                vale += [j]


    return tuple(ordena_intersecoes(vale))



'''
Duas interseções estão conectadas se pertencerem à mesma cadeia
(quer sejam montanhas ou livres)

Na verificação dos dados quero verificar que:
o território é, de facto, um território;
que ambas as interseções são realmente interseções;
que ambas as interseções estão dentro do território dado
'''
def verifica_conexao(territ, inters1, inters2):
    if not eh_territorio(territ) or not eh_intersecao(inters1)\
    or not eh_intersecao_valida(territ, inters1) or not eh_intersecao(inters2)\
    or not eh_intersecao_valida(territ, inters2):
        raise ValueError("verifica_conexao: argumentos invalidos")


    #verificar se a inters2 está na cadeia da inters1
    #(vice-versa é equivalente)
    return inters2 in obtem_cadeia(territ, inters1)



'''
Função auxiliar #4

Função auxiliar criada para ajudar na função calcula_numero_cadeias_montanhas.
Como o código original das funções calcula_numero_montanhas e
calcula_tamanho_vales utilizava o código desta função auxiliar,
adaptou-se o código para utilizar esta função auxiliar


É feito o mesmo raciocínio e os mesmos loops que na função territorio_para_str
Contudo, o código não é exatamente igual, pelo que não me é útil usar
esta função auxiliar na função territorio_para_str

Tal como na função territorio_para_str, itero decrescentemente numa range
que começa no comprimento das linhas verticais e acaba em 1
(não há coordenada 0, depois ajusto com -1 para trabalhar com os índices)
Assim, consigo acompanhar a coordenada da linha horizontal à medida que
faço o segundo loop, onde itero por cada subtuplo (linha vertical)

Ao combinar as coordenadas consigo aceder ao valor de cada interseção
e saber se tem uma montanha ou não. Se tiver adiciono a interseção à lista
'''
def calcula_montanhas(territ):
    montanhas = []

    for i in range(len(territ[0]), 0, -1):
        for j in range(len(territ)):
            
            if territ[j][i-1] == 1:
                montanhas += ((num_p_letra(j+1), i),)


    return tuple(ordena_intersecoes(montanhas))



'''
A função calcula_montanhas retorna um tuplo com todas as montanhas
e o comprimento desse tuplo é o número de montanhas do território
'''
def calcula_numero_montanhas(territ):
    if not eh_territorio(territ):
        raise ValueError("calcula_numero_montanhas: argumento invalido")
    
    return len(calcula_montanhas(territ))



'''
Uso a função calcula_montanhas para ter todas as montanhas do território
Para cada montanha vou buscar a cadeia a que pertence e,
se ainda não estiver na lista de cadeias, adiciono-a

Contudo, para evitar tempos de execução muito longos para
territórios muito grandes, por cada cadeia retiro da lista de montanhas
todas as montanhas que pertencem a essa cadeia (exceto a montanha
que foi usada para calcular a cadeia para o loop não saltar elementos)
'''
def calcula_numero_cadeias_montanhas(territ):
    if not eh_territorio(territ):
        raise ValueError("calcula_numero_cadeias_montanhas: argumento invalido")


    l_cadeias = []
    montanhas = list(calcula_montanhas(territ))
    
    for i in montanhas:
        cadeia = obtem_cadeia(territ, i)
        
        for j in cadeia:
            if j != i:
                montanhas.remove(j)

        '''
        inicialmente tinha a condição if cadeia not in l_cadeias:
        mas é desnecessário verificar isto pois como removemos
        as montanhas da lista nunca iremos obter cadeias repetidas
        '''
        l_cadeias += (cadeia,)


    return len(l_cadeias)



'''
Para todas as montanhas que existem no território, vou buscar todas
as interseções dos vales de cada uma e adiciono-as à lista dos vales
apenas se ainda não estiverem lá

Acabo com uma lista que contém todas as interseções do território
que pertencem a pelo menos um vale e sem interseções repetidas
Basta-me devolver o comprimento da lista para obter
o número de interseções do território que pertencem a pelo menos um vale
'''
def calcula_tamanho_vales(territ):
    if not eh_territorio(territ):
        raise ValueError("calcula_tamanho_vales: argumento invalido")


    vales = []

    for i in calcula_montanhas(territ):
        for j in obtem_vale(territ, i):

            if j not in vales:
                vales += (j,)


    return len(vales)



'''
FIM DO PROJETO
'''
