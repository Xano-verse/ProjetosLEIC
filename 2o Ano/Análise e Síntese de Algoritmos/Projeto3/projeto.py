from pulp import *
from collections import defaultdict


fmax = {} # dicionario com chave i (fabrica) e valor fmaxi (stock maximo da fabrica)
pmin = {} # dicionario com chave j (pais) e valor pminj (minimo de brinquedos a distribuir nesse pais)
pmax = {} # dicionario com chave j (pais) e valor pmaxj (maximo de exportacao desse pais)
pais_fabrica = {} # dicionario com chave i (fabrica) e valor j (pais correspondente a essa fabrica)
pedidos_por_fabrica = defaultdict(list) # dicionario com chave i (fabrica) e valor [pedidos que essa fabrica tem]
pedidos_por_exportacao = defaultdict(list) # dicionario com chave j (pais) e valor [pedidos que pode exportar]
                                           # Por exemplo, pais 1 pode exportar x_2_1 se fabrica 1 estiver no pais 1 SE crianca 2 nao estiver no pais 1
pedidos_por_entrega = defaultdict(list) # dicionario com chave j (pais da crianca) e valor [pedidos que esse pais tem]
                                        # Por exemplo, pais 1 recebe pedido x_2_1 se a crianca 2 estiver no pais 1
pedidos_por_crianca = defaultdict(list) # dicionario com chave k (crianca) e valor [pedidos desse crianca]

#*******************************************
#                 PROBLEMA                 #
#*******************************************
prob = LpProblem("P3", LpMaximize)

# n fabricas, m paises, t criancas
n, m, t = tuple(map(int, input().split()))

for _ in range(n):
    # i identificador da fabrica, j identificador do pais, fmaxi stock maximo da fabrica
    i, j, fmaxi = tuple(map(int, input().split()))
    # apenas considerar fabricas com fmax > 0, se for 0 ou negativo saltamos
    if fmaxi > 0:
        fmax[i] = fmaxi
        pais_fabrica[i] = j

for _ in range(m):
    # j identificador do pais, pmaxj maximo de exportacao desse pais, pminj minimo de brinquedos a distribuir nesse pais
    j, pmaxj, pminj = tuple(map(int, input().split()))
    pmax[j] = pmaxj
    pmin[j] = pminj

for _ in range(t):
    lst = tuple(map(int, input().split()))

    # k identificador da crianca
    k = lst[0]
    # j identificador do pais da crianca
    j = lst[1]    
    # percorrer lista de brinquedos da crianca e, se for duma fabrica valida, adicionar a lista
    fabricas_validas = [i for i in lst[2:] if i in fmax]

    # dicionario com as variaveis todas, uma para cada par (crianca, fabrica do brinquedo)
    # (a crianca muda a cada iteracao do outer loop)
    # cada crianca tem um dicionario x_variables com todos os seus pedidos validos como chaves
    #*******************************************
    #           VARIAVEL DO PROBLEMA           #
    #*******************************************
    """ x_variables[(k, i)]: Variavel binaria que indica se o
        pedido da criança k sera atendido pela fabrica i (1 ou 0) """
    x_variables = LpVariable.dicts("x_variables", [(k, i) for i in fabricas_validas], 0, 1, 'Binary')

    # para cada fabrica, ou seja, para cada brinquedo da crianca em que estou
    # vamos ver os pedidos desta crianca
    for i in fabricas_validas:
        # acedo a variavel correspondente ao pedido
        x = x_variables[(k, i)]

        # adicionar a este dicionario e direto, a crianca k tem o pedido (k, i)
        pedidos_por_crianca[k].append(x)
        
        # este tambem, a fabrica do brinquedo i tem o pedido (k, i) 
        pedidos_por_fabrica[i].append(x)
        
        # se a crianca esta num pais diferente do que a fabrica deste pedido
        if pais_fabrica[i] != j:
            # entao ha uma possivel exportacao, o pais da fabrica do pedido vai ter este pedido
            pedidos_por_exportacao[pais_fabrica[i]].append(x)
        
        # e direto, o pais da crianca tem este pedido da crianca
        pedidos_por_entrega[j].append(x)

#*******************************************
#              FUNCAO OBJETIVO             #
#*******************************************
""" Maximizar a soma das variaveis associadas aos pedidos de todas as criancas,
ou seja, maximizar o nº total de pedidos atendidos """ 
prob += lpSum(pedidos_por_crianca[k] for k in range(1, t+1))

#*******************************************
#          RESTRICOES DO PROBLEMA          #
#*******************************************
"""
1. Restricao do stock maximo por fábrica (cada fabrica tem um stock max fmax)
2. Restricao da exportacao por pais (cada pais tem um valor maximo pamx de exportacoes)
3. Restricao da entrega minima por pais (cada pais tem um valor min pmin de entregas)
4. Restricao do nº de pedidos atendidos por crianca (cada crianca so receber, no max, um brinquedo)
"""
maximo = max(n, m, t)
for z in range(1, maximo+1):
    if z <= n and z in fmax:
            # so ponho a restricao se houver mais pedidos para cada fabrica do que o seu stock 
            if len(pedidos_por_fabrica[z]) > fmax[z]:
                prob += lpSum(pedidos_por_fabrica[z]) <= fmax[z] # Restricao 1
    
    if z <= m:
        # so ponho a restricao se houver mais pedidos de exportacao do que a capacidade do pais
        if len(pedidos_por_exportacao[z]) > pmax[z]:
            prob += lpSum(pedidos_por_exportacao[z]) <= pmax[z] # Restricao 2

        prob += lpSum(pedidos_por_entrega[z]) >= pmin[z] # Restricao 3

    if z <= t:
        # so ponho a restricao se a crianca tiver mais do que 1 pedido
        if len(pedidos_por_crianca[z]) > 1:
            prob += lpSum(pedidos_por_crianca[z]) <= 1 # Restricao 4


prob.solve(GLPK_CMD(msg=False))

if LpStatus[prob.status] == "Optimal":
    print(int(value(prob.objective)))
else:
    print(-1)
 
