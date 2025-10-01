set.seed(2054)  # Fixar a semente para reprodutibilidade

n <- 45000

# Simular 3 dados por jogada, 45000 jogadas
dados <- matrix(sample(1:6, 3 * n, replace = TRUE), nrow = n)

# Somar os valores dos 3 dados em cada jogada
somas <- rowSums(dados)

# Frequências relativas
freq_9 <- mean(somas == 9)
freq_10 <- mean(somas == 10)

# Diferença entre frequências relativas
diff <- abs(freq_10 - freq_9)

# Arredondar a 4 casas decimais
round(diff, 4)

