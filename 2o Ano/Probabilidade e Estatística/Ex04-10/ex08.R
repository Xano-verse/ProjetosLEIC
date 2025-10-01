set.seed(1133)

# Parâmetros
mu <- 0.3
sigma <- 0.6
n <- 14
m <- 1500
gamma <- 0.91
alpha <- 1 - gamma
z <- qnorm(1 - alpha / 2)

# Gerar amostras
samples <- matrix(rnorm(n * m, mean = mu, sd = sigma), nrow = m)

# Calcular médias de cada amostra
sample_means <- rowMeans(samples)

# Margem de erro
error_margin <- z * sigma / sqrt(n)

# Construir ICs
lower_bounds <- sample_means - error_margin
upper_bounds <- sample_means + error_margin

# Verificar quais ICs contêm o verdadeiro mu
contains_mu <- (lower_bounds <= mu) & (mu <= upper_bounds)

# Proporção de ICs que contêm o verdadeiro valor
proportion <- mean(contains_mu)

# Quociente entre a proporção obtida e o nível de confiança
quotient <- proportion / gamma

# Resultados
cat("Proporção de ICs que contêm mu:", round(proportion, 4), "\n")
cat("Quociente entre proporção e nível de confiança:", round(quotient, 4), "\n")

