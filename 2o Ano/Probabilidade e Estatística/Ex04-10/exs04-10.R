# ======================================================================================== 
# Ex 4

# Valor esperado de X
lambda <- 22
k <- 10
expected_value_x <- lambda * gamma(1 + 1/k)
expected_value_x


# Calcular E(X) aproximado, 9000 amostras, semente em 530
set.seed(530)
x <- rweibull(9000, shape = k, scale = lambda)
expected_value_ex <- mean(x)
expected_value_ex


# Diferenca entre os dois valores
difference <- abs(expected_value_x - expected_value_ex)
difference <- round(difference, 4)
difference




# ======================================================================================== 
# Ex 5

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




# ======================================================================================== 
# Ex 6

# Valor exato de pn=P(Sn≤5.7)
n <- 12
x <- 5.7
pn_exact <- 0
for (k in 0:floor(x)) {
  pn_exact <- pn_exact + (-1)^k * choose(n, k) * (x - k)^n
}
pn_exact <- pn_exact / factorial(n)
pn_exact

# Aproximação pn via Teorema do Limite Central (TLC)
pn_tlc <- pnorm(-0.3)
pn_tlc

# Aproximação pn por simulação
set.seed(4301)
n <- 12
m <- 100
sim <- replicate(m, sum(runif(n)))
pn_sim <- mean(sim <= 5.7)
pn_sim


# Desvio absoluto entre o valor exacto pn e o valor aproximado pn,TLC
desv_abs_1 <- abs(pn_exact - pn_tlc)

# Desvio absoluto entre pn e pn,sim
desv_abs_2 <- abs(pn_exact - pn_sim)

# Quociente
quoc <- desv_abs_1 / desv_abs_2
quoc <- round(quoc, 4)
quoc




# ======================================================================================== 
# Ex 7

# Dados
n <- 13
sum_x <- 119.68
sum_logx <- 28.82

# Constante do lado direito
C <- log(sum_x / n) - (sum_logx / n)

# Função a resolver
f <- function(alpha) log(alpha) - digamma(alpha) - C

# Encontrar raiz em [0.001, 169.8]
alpha_hat <- uniroot(f, interval = c(0.001, 169.8))$root

# Estimar lambda
lambda_hat <- n * alpha_hat / sum_x

# Calcular moda
mode <- (alpha_hat - 1) / lambda_hat

# Mostrar resultado arredondado
mode <- round(mode, 2)

mode




# ======================================================================================== 
# Ex 8

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

quotient <- round(quotient, 4)

# Quociente entre proporção e nível de confiança:
quotient




# ======================================================================================== 
# Ex 9

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


quotient <- beta_hat / beta

quotient <- round(quotient, 4)

# Quociente entre beta_had e beta
quotient




# ======================================================================================== 
# Ex 10

set.seed(3379)

dados <- c(2.5, 0.9, 2.2, 1.9, 3.2, 4.5, 1.6, 1.7, 4.4, 1, 0.8, 4.3, 1.4, 1.9, 2.3, 0.8, 4.3, 2.6, 5.5, 0.3, 4.8, 2.6, 3, 2.2, 2.2, 1.8, 3.5, 2.8, 3.5, 3.2, 2, 1.8, 1.4, 6.6, 0.9, 2.1, 1.5, 4, 0.8, 0.8, 3.2, 2, 2.5, 3.6, 3.6, 2.3, 2.6, 3.7, 3, 1.1, 1.1, 1, 1.5, 1.6, 3, 0.9, 0.6, 2.2, 3.7, 1.5, 1.2, 2.2, 2.7, 2.7, 3.2, 3.5, 2.7, 3.3, 2.9, 1.6, 3, 4.2, 3.4, 5.7, 0.2, 4.4, 5.5, 1.7, 1.6, 1, 2.8, 1.5, 2.2, 1.8, 1.1, 4, 4.9, 1.6, 2.6, 2.3, 2.1, 2.5, 2.6, 1.5, 0.9, 1.1, 7.1, 5, 0.8, 0.7, 2.4, 4.3, 2.1, 4.5, 2.8, 4.4, 2.7, 2.5, 1.3, 6.6, 2.1, 2.4, 3.2, 1.1, 2.2, 1.7, 2.1, 4.2, 4.2, 0.9, 2, 1.9, 1.5, 4.6, 3.1, 4.1, 4.1, 3.9, 2.2, 2, 3.2, 1.4, 2.6, 5.1, 2.7, 5.4, 1.5, 2.9, 1.8, 4.6, 3.2, 0.9, 2.5, 3, 2.2, 2.2, 5.1, 4.3, 0.9, 0.5, 1.5, 1.7, 2.9, 1.3, 2, 1.5, 1.1, 2.6, 3.1, 2.6, 4.1, 2, 2.8, 2.9, 1.1, 1.9, 2.2, 2.5, 3.2, 1.7, 2.5, 3.8, 3.2, 1.6, 0.9, 1.5, 1.2, 2.5, 2.6, 2.4, 1.1, 6.1, 2.3, 1.4, 2.3, 2.6, 2.8, 3.3, 0.3, 5.3, 2.8, 2.1, 2.3, 1.1, 1.7, 2.1, 0.7, 1, 3.9, 2.4, 3.6, 0.8, 2.6, 0.9, 2.1, 4.5, 5.4, 3.8, 4.3, 2.1)


# 1. Subamostra de 168 sem reposição
subamostra <- sample(dados, size = 168, replace = FALSE)

# 2. Divisão do suporte em 8 classes equiprováveis sob H0 (Rayleigh(sigma = 2.2))
sigma <- 2.2
k <- 8
probs <- seq(0, 1, length.out = k + 1)
breaks <- sqrt(-2 * sigma^2 * log(1 - probs))  # Inversa da CDF de Rayleigh

# 3. Frequências observadas
obs_freq <- hist(subamostra, breaks = breaks, plot = FALSE)$counts

# 4. Teste do qui-quadrado
exp_freq <- rep(length(subamostra) / k, k)
test_stat <- sum((obs_freq - exp_freq)^2 / exp_freq)
p_value <- pchisq(test_stat, df = k - 1, lower.tail = FALSE)

p_value <- round(p_value, 4)

p_value


