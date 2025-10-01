set.seed(4606)

# Parâmetros
mu0 <- 4
mu1 <- 4.5
n <- 26
m <- 900
alpha <- 0.08
df <- 2 * n
critical_value <- qchisq(1 - alpha, df = df)

# Simular amostras sob H1: mu = mu1
samples <- matrix(rexp(n * m, rate = 1 / mu1), nrow = m)

# Médias amostrais
sample_means <- rowMeans(samples)

# Estatística de teste sob H0
T0_values <- (2 * n * sample_means) / mu0

# Erro de 2ª espécie: não rejeita H0 quando H1 é verdadeira
non_rejections <- T0_values <= critical_value
beta_hat <- mean(non_rejections)

# Calcular beta teórico (probabilidade de T0 <= critical_value sob mu1)
# Sob H1, T0 ~ (mu1/mu0) * chi2(2n)
# Então: P(T0 <= critical) = P(chi2 <= critical * mu0/mu1)
adjusted_critical <- critical_value * mu0 / mu1
beta <- pchisq(adjusted_critical, df = df)

# Quociente
quotient <- beta_hat / beta

# Resultados
cat("Beta estimado (simulado):", round(beta_hat, 4), "\n")
cat("Beta teórico:", round(beta, 4), "\n")
cat("Quociente entre beta_hat e beta:", round(quotient, 4), "\n")

