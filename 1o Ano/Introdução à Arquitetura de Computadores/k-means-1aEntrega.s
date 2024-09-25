#
# IAC 2023/2024 k-means
# 
# Grupo: 63
# Campus: Alameda
#
# Autores:
# 109441, Alexandre Delgado
# 109704, Guilherme Jose
# 109851, Francisco Goncalves
#
# Tecnico/ULisboa


# ALGUMA INFORMACAO ADICIONAL PARA CADA GRUPO:
# - A "LED matrix" deve ter um tamanho de 32 x 32
# - O input e' definido na seccao .data. 
# - Abaixo propomos alguns inputs possiveis. Para usar um dos inputs propostos, basta descomentar 
#   esse e comentar os restantes.
# - Encorajamos cada grupo a inventar e experimentar outros inputs.
# - Os vetores points e centroids estao na forma x0, y0, x1, y1, ...


# Variaveis em memoria
.data

#Input A - linha inclinada
#n_points:    .word 9
#points:      .word 0,0, 1,1, 2,2, 3,3, 4,4, 5,5, 6,6, 7,7 8,8

#Input B - Cruz
#n_points:    .word 5
#points:      .word 4,2, 5,1, 5,2, 5,3 6,2

#Input C
#n_points:    .word 23
#points: .word 0,0, 0,1, 0,2, 1,0, 1,1, 1,2, 1,3, 2,0, 2,1, 5,3, 6,2, 6,3, 6,4, 7,2, 7,3, 6,8, 6,9, 7,8, 8,7, 8,8, 8,9, 9,7, 9,8

#Input D
n_points:    .word 30
points:      .word 16, 1, 17, 2, 18, 6, 20, 3, 21, 1, 17, 4, 21, 7, 16, 4, 21, 6, 19, 6, 4, 24, 6, 24, 8, 23, 6, 26, 6, 26, 6, 23, 8, 25, 7, 26, 7, 20, 4, 21, 4, 10, 2, 10, 3, 11, 2, 12, 4, 13, 4, 9, 4, 9, 3, 8, 0, 10, 4, 10



# Valores de centroids e k a usar na 1a parte do projeto:
centroids:   .word 0,0
k:           .word 1

# Valores de centroids, k e L a usar na 2a parte do prejeto:
#centroids:   .word 0,0, 10,0, 0,10
#k:           .word 3
#L:           .word 10

# Abaixo devem ser declarados o vetor clusters (2a parte) e outras estruturas de dados
# que o grupo considere necessarias para a solucao:
#clusters:    




#Definicoes de cores a usar no projeto 

colors:      .word 0xff0000, 0x00ff00, 0x0000ff  # Cores dos pontos do cluster 0, 1, 2, etc.

.equ         black      0
.equ         white      0xffffff
.equ LED_MATRIX_WIDTH, 32   # Definir tamanho
.equ LED_MATRIX_HEIGHT, 32  # Definir tamanho



# Codigo
 
.text
    # Chama funcao principal da 1a parte do projeto
    jal ra, mainSingleCluster
    
    # Descomentar na 2a parte do projeto:
    #jal mainKMeans
    
    #Termina o programa (chamando chamada sistema)
    li a7, 10
    ecall



### printPoint
# Pinta o ponto (x,y) na LED matrix com a cor passada por argumento
# Nota: a implementacao desta funcao ja' e' fornecida pelos docentes
# E' uma funcao auxiliar que deve ser chamada pelas funcoes seguintes que pintam a LED matrix.
# Argumentos:
# a0: x
# a1: y
# a2: cor

printPoint:
        li a3, LED_MATRIX_0_HEIGHT
        sub a1, a3, a1
        addi a1, a1, -1
        li a3, LED_MATRIX_0_WIDTH
        mul a3, a3, a1
        add a3, a3, a0
        slli a3, a3, 2
        li a0, LED_MATRIX_0_BASE
        add a3, a3, a0   # addr
        sw a2, 0(a3)
        jr ra


    
### cleanScreen
# Limpa todos os pontos do ecra
# Argumentos: nenhum
# Retorno: nenhum

cleanScreen:
    addi sp, sp, -4             # Criar espaco no stack
    sw ra, 0(sp)                # Guardar o return adress no stack
    li a2, white                # Carregar a cor para a2
    li t0, 31                   # Carregar o numero de pixeis nas colunas (0-31)
    linhas:
        li t1 31                # Carregar o numero de pixeis nas linhas (0-31)
        colunas:
            mv a0, t0           # Mover t0 para um argumento 
            mv a1, t1           # Mover t1 para um argumento
            jal printPoint      # Chamar a funcao printPoint com os argumentos (a0, a1, a2)
            addi t1, t1, -1     # Reduzir uma posicao
            bgez t1, colunas    # Se ainda existir uma posicao seguinte, chamar loop novamente
        
        addi t0, t0 -1          # Reduzir uma posicao
        bgez t0, linhas         # Se ainda existir uma posicao seguinte, chamar loop novamente
    lw ra, 0(sp)                # Carregar o return address inicial
    addi sp, sp, 4              # Restaurar o stack para o seu estado inicial
    jr ra                       # Return



### printFromVector:
# FUNCAO AUXILIAR
# Percorre um vetor de pontos e pinta-os na LED matrix
# Argumentos:
# a0: endereco do vetor
# a1: numero de pontos a pintar
# a2: cor para os pontos
# Retorno: nenhum

printFromVector:
    mv t0, a0                                   # t0 = endereco base do vetor
    mv t1, a1                                   # t1 = numero de pontos
    slli t1, t1, 1                              # t1 = numero de pontos * 2 = numero de elementos no vetor
    li t2, 0                                    # t2 = i
    
    for_printFromVector:
        bge t2, t1, skip_for_printFromVector    # Loop for corre apenas enquanto i < numero de elementos do vetor
        
        slli t3, t2, 2                          # i*4
        add t4, t0, t3                          # Endereco base + (i*4)
        lw a0, 0(t4)                            # Coordenada x e o primeiro argumento do printPoint
        lw a1, 4(t4)                            # Coordenada y e o segundo argumento
        
        #A cor ja esta no a2
        
        #(printPoint nao interfere com registos temporarios)
        addi sp, sp, -4                         # Criar espaco no stack
        sw ra, 0(sp)                            # Guardar endereco de retorno

        jal ra, printPoint                      # Pinta ponto na matriz
        
        lw ra, 0(sp)                            # Recuperar endereco de retorno
        addi sp, sp, 4                          # Restaurar a stack para o seu estado inicial
        
        
        addi t2, t2, 2                          # Proximo x esta em i+2
        j for_printFromVector
        
    skip_for_printFromVector:
        jr ra
        

    
### printClusters
# Pinta os agrupamentos na LED matrix com a cor correspondente.
# Argumentos: nenhum
# Retorno: nenhum

printClusters:
    la a0, points              # Endereco do vetor points e o primeiro argumento
    mv a1, s0                  # Numero de pontos a pintar e o segundo argumento
    li a2, 0xff0000            # Cor e o terceiro argumento
    
    
    addi sp, sp, -4            # Criar espaco no stack
    sw ra, 0(sp)               # Guardar endereco de retorno
    
    jal ra, printFromVector    # Pinta os pontos do vetor na matriz
    
    lw ra, 0(sp)               # Recuperar endereco de retorno
    addi sp, sp, 4             # Restaurar a stack para o seu estado inicial
    
    
    jr ra
  


### printCentroids
# Pinta os centroides na LED matrix
# Nota: deve ser usada a cor preta (black) para todos os centroides
# Argumentos: nenhum
# Retorno: nenhum

printCentroids:
    la a0, centroids           # Endereco do vetor centroids e o primeiro argumento
    la a1, k                
    lw a1, 0(a1)               # Numero de pontos a pintar e o segundo argumento
    li a2, black               # Cor e o terceiro argumento
    
    
    addi sp, sp, -4            # Criar espaco no stack
    sw ra, 0(sp)               # Guardar endereco de retorno
    
    jal ra, printFromVector    # Pinta os pontos do vetor na matriz
    

    lw ra, 0(sp)               # Recuperar endereco de retorno
    addi sp, sp, 4             # Restaurar a stack para o seu estado inicial
    
    
    jr ra
    
    
    
### calculateCentroids
# Calcula os k centroides, a partir da distribuicao atual de pontos associados a cada agrupamento (cluster)
# Argumentos: nenhum
# Retorno: nenhum

calculateCentroids:
    li t0, 0                                   # Inicializa soma das coordenadas x
    li t1, 0                                   # Inicializa soma das coordenadas y
    la t3, points                              # Load do address do vetor points
    mv t4, s0                                  # Copia do numero de pontos

    loop_calculateCentroids:
        beqz t4, finish_calculateCentroids     # Verifica se ha pontos a analisar
        lw t5, 0(t3)                           # Load da coordenada x
        add t0, t0, t5                         # Adiciona coordenada x a soma
        lw t6, 4(t3)                           # Load da coordenada y
        add t1, t1, t6                         # Adiciona coordenada y a soma
        addi t3, t3, 8                         # Muda o address para o proximo ponto
        addi t4, t4, -1                        # Decrementa o numero de pontos que ainda falta
        j loop_calculateCentroids              # Volta ao inicio

    finish_calculateCentroids:
        div t0, t0, s0                         # Calcula media das coordenadas x
        div t1, t1, s0                         # Calcula media das coordenadas y
        la x28, centroids                      # Load do address dos centroides
        sw t0, 0(x28)                          # Guarda a coordenada x 
        sw t1, 4(x28)                          # Guarda a coordenada y
        jr ra                                  # Termina



### mainSingleCluster
# Funcao principal da 1a parte do projeto.
# Argumentos: nenhum
# Retorno: nenhum

mainSingleCluster:

    #1. Coloca k=1 (caso nao esteja a 1)
    la t0, k
    li t1, 1
    sw t1, 0(t0)

    # Guarda endereco n_points em s0 pois vai ser usado varias vezes
    la s0, n_points
    lw, s0, 0(s0)
    
    #2. cleanScreen
    addi sp, sp, -4        # Criar espaco no stack
    sw ra, 0(sp)           # Guardar endereco de retorno
    
    jal ra, cleanScreen


    #3. printClusters
    jal ra, printClusters

    
    #4. calculateCentroids    
    jal ra, calculateCentroids


    #5. printCentroids
    jal ra, printCentroids
   
    lw ra, 0(sp)           # Recuperar endereco de retorno
    addi sp, sp, 4         # Restaurar a stack para o seu estado inicial


    #6. Termina
    jr ra







# O QUE ESTA EM BAIXO E APENAS PARA A 2a PARTE, NAO AVALIAR


### manhattanDistance
# Calcula a distancia de Manhattan entre (x0,y0) e (x1,y1)
# Argumentos:
# a0, a1: x0, y0 
# a2, a3: x1, y1
# Retorno:
# a0: distance

manhattanDistance:
    # POR IMPLEMENTAR (2a parte)
        sub a0, a2, a0
        sub a1, a3, a1
        bge a0, x0, Skip
        bge a1, x0, Skip
        
        not a0, a0
        addi a0, a0, 1
        not a1, a1
        addi a1, a1, 1
        
        Skip:
                add, a0, a0, a1
                jr ra
        



### nearestCluster
# Determina o centroide mais perto de um dado ponto (x,y).
# Argumentos:
# a0, a1: (x, y) point
# Retorno:
# a0: cluster index

nearestCluster:
    # POR IMPLEMENTAR (2a parte)
    jr ra


### mainKMeans
# Executa o algoritmo *k-means*.
# Argumentos: nenhum
# Retorno: nenhum

mainKMeans:  
    # POR IMPLEMENTAR (2a parte)
    jr ra
