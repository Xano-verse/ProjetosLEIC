# Código R para as Questões de Estatística
# ------------------------------------------------------

# Questão 1: Valor Esperado de Distribuição de Weibull
# ------------------------------------------------------

# Parâmetros da distribuição de Weibull
lambda <- 22
k <- 10

# Cálculo do valor esperado usando a função gamma
E_X_exato <- lambda * gamma(1 + 1/k)
cat("Valor esperado exato: ", E_X_exato, "\n")

# Fixar a semente para reprodutibilidade
set.seed(530)

# Gerar amostra de dimensão 6000 da distribuição de Weibull
amostra <- rweibull(9000, shape = k, scale = lambda)

# Calcular a média amostral como aproximação do valor esperado
E_X_aprox <- mean(amostra)
cat("Valor esperado aproximado (Monte Carlo): ", E_X_aprox, "\n")

# Calcular o desvio absoluto entre os dois métodos
desvio <- abs(E_X_exato - E_X_aprox)
cat("Desvio absoluto: ", desvio, "\n")


# Questão 2: Jogo do Grão-Duque da Toscana
# ------------------------------------------------------

# Fixar a semente conforme solicitado
set.seed(2054)

# Número de jogadas
n <- 45000

# Contadores para as somas
soma_9 <- 0
soma_10 <- 0

# Simular n jogadas
for (i in 1:n) {
  # Lançar 3 dados (sem especificar probabilidades)
  dados <- sample(1:6, size = 3, replace = TRUE)
  soma <- sum(dados)
  
  # Verificar o resultado
  if (soma == 9) {
    soma_9 <- soma_9 + 1
  } else if (soma == 10) {
    soma_10 <- soma_10 + 1
  }
}

# Calcular as frequências relativas
freq_9 <- soma_9 / n
freq_10 <- soma_10 / n

# Calcular a diferença entre as frequências
diferenca <- freq_10 - freq_9

# Resultado arredondado a 4 casas decimais
diferenca_arredondada <- round(diferenca, 4)

cat("Frequência relativa de soma 9:", freq_9, "\n")
cat("Frequência relativa de soma 10:", freq_10, "\n")
cat("Diferença (soma 10 - soma 9):", diferenca_arredondada, "\n")


# Questão 3: Distribuição de Irwin-Hall
# ------------------------------------------------------

# 1. Valor exato de p_n
n <- 12
x <- 5.7
floor_x <- floor(x)
soma <- 0
for (k in 0:floor_x) {
  termo <- (-1)^k * choose(n, k) * (x - k)^n
  soma <- soma + termo
}
pn_exato <- soma / factorial(n)

# 2a. Aproximação pelo TLC
media_tlc <- n * 0.5
dp_tlc <- sqrt(n / 12)
pn_tlc <- pnorm(x, media_tlc, dp_tlc)

# 2b. Simulação
set.seed(4301)
m <- 100
amostras <- matrix(runif(m * n), nrow = m)
somas <- rowSums(amostras)
pn_sim <- mean(somas <= x)

# 3. Desvio absoluto (TLC)
desvio_tlc <- abs(pn_exato - pn_tlc)

# 4. Desvio absoluto (Simulação)
desvio_sim <- abs(pn_exato - pn_sim)

# 5. Quociente arredondado
quociente <- round(desvio_tlc / desvio_sim, 4)

# Resultados
cat("1. Valor exato:", pn_exato, "\n")
cat("2a. TLC:", pn_tlc, "\n")
cat("2b. Simulação:", pn_sim, "\n")
cat("3. Desvio TLC:", desvio_tlc, "\n")
cat("4. Desvio Simulação:", desvio_sim, "\n")
cat("5. Quociente:", quociente, "\n")


# Questão 4: Estimativa de Máxima Verossimilhança para Distribuição Gamma
# --------------------------------------------------------------------

# Dados fornecidos
n <- 13
sum_x <- 119.68
sum_log_x <- 28.82

# Função de log-verossimilhança para encontrar alpha
# Baseada na derivação da log-verossimilhança em relação a alpha e lambda
loglik_equation <- function(alpha) {
  # Sabemos que lambda_hat = n*alpha/sum_x
  # Substituindo na equação derivada da log-verossimilhança:
  n * log(n * alpha / sum_x) - n * digamma(alpha) + sum_log_x
}

# Usando uniroot para encontrar alpha numericamente no intervalo [17.6, 26.4]
alpha_hat <- uniroot(loglik_equation, interval = c(0.001, 169.8))$root

# Calculando lambda_hat usando a equação lambda_hat = n*alpha/sum_x
lambda_hat <- n * alpha_hat / sum_x

# Calculando o comprimento modal (alpha-1)/lambda
# Na distribuição Gamma, o modo é (alpha-1)/lambda para alpha > 1
comprimento_modal <- (alpha_hat - 1) / lambda_hat

# Resultado final arredondado a 2 casas decimais
resultado <- round(comprimento_modal, 2)

cat("Estimativa de máxima verossimilhança de alpha:", alpha_hat, "\n")
cat("Estimativa de máxima verossimilhança de lambda:", lambda_hat, "\n")
cat("Comprimento modal dos ovos:", resultado, "cm\n")


# Questão 5: Intervalos de Confiança para Distribuição Normal
# --------------------------------------------------------------------

# Parâmetros do problema
set.seed(1133)
m <- 1500# número de amostras
n <- 14# tamanho de cada amostra
mu <- 0.3# valor esperado verdadeiro
sigma <- 0.6# desvio padrão conhecido
gamma <- 0.91# nível de confiança

# Valor crítico z para o nível de confiança gamma
z_critical <- qnorm((1 + gamma) / 2)

# Inicializar contador para intervalos que contêm mu
count_contains_mu <- 0

# Gerar m amostras e calcular intervalos de confiança
for (i in 1:m) {
  # Gerar uma amostra de tamanho n da distribuição normal
  amostra <- rnorm(n, mean = mu, sd = sigma)
  
  # Calcular a média amostral
  media_amostral <- mean(amostra)
  
  # Calcular os limites do intervalo de confiança
  limite_inferior <- media_amostral - z_critical * sigma / sqrt(n)
  limite_superior <- media_amostral + z_critical * sigma / sqrt(n)
  
  # Verificar se o intervalo contém o valor verdadeiro de mu
  if (limite_inferior <= mu && mu <= limite_superior) {
    count_contains_mu <- count_contains_mu + 1
  }
}

# Calcular a proporção de intervalos que contêm mu
proporcao <- count_contains_mu / m

# Calcular o quociente entre a proporção e o nível de confiança
quociente <- proporcao / gamma

# Arredondar o quociente a 4 casas decimais
quociente_arredondado <- round(quociente, 4)

cat("Proporção de intervalos que contêm mu:", proporcao, "\n")
cat("Quociente entre a proporção e gamma:", quociente_arredondado, "\n")


# Questão 6: Teste de Hipóteses para Distribuição Exponencial
# --------------------------------------------------------------------

# Parâmetros do problema
set.seed(4606)
m <- 900# número de amostras
n <- 26# dimensão de cada amostra
mu0 <- 4       # valor esperado sob H0
mu1 <- 4.5       # valor esperado sob H1
alpha <- 0.08# nível de significância

# Valor crítico para rejeição de H0
critical_value <- qchisq(1 - alpha, df = 2*n)

# Cálculo teórico de beta (probabilidade do erro de 2ª espécie)
# Quando os dados são da distribuição com mu = mu1, mas testamos mu = mu0:
# T0 = (2n*X_barra)/mu0 = (mu1/mu0)*[(2n*X_barra)/mu1]
# A distribuição de (2n*X_barra)/mu1 é qui-quadrado com 2n graus de liberdade
beta_theoretical <- pchisq((mu0/mu1) * critical_value, df = 2*n)

# Simulação para estimar beta
count_not_reject_H0 <- 0

for (i in 1:m) {
  # Gerar amostra exponencial com média mu1
  sample_data <- rexp(n, rate = 1/mu1)
  
  # Calcular estatística T0 sob H0
  T0 <- (2*n * mean(sample_data)) / mu0
  
  # Verificar se não rejeitamos H0 (erro de 2ª espécie)
  if (T0 <= critical_value) {
    count_not_reject_H0 <- count_not_reject_H0 + 1
  }
}

# Estimativa empírica de beta
beta_estimate <- count_not_reject_H0 / m

# Quociente entre beta_estimate e beta_theoretical
ratio <- beta_estimate / beta_theoretical
rounded_ratio <- round(ratio, 4)

cat("Beta teórico:", beta_theoretical, "\n")
cat("Beta estimado:", beta_estimate, "\n")
cat("Quociente (beta_estimate/beta_theoretical):", rounded_ratio, "\n")


# Questão 7: Teste de Ajustamento para Distribuição de Rayleigh
# --------------------------------------------------------------------

# Este é um exemplo de como eu estruturaria o código para esta questão
# Os dados completos foram omitidos por brevidade

# Organizar os dados fornecidos (primeiros e últimos elementos)
#dados <- c(2.3, 2.7, 5.2, 0.7, 2.9, 0.6, 2.6, 2.2, 3.8, 0.5, 4.9, 5.4,
#3.7, 0.4, 4, 3.6, 2, 0.8, 2.5, 2.8, 1.7, 3.3, 1.5, 0.4, 6.4, 1.5, 6, 
  #2.1, 0.4, 4.6, 3.1, 4.4, 4, 2.1, 5, 3.3, 4.7, 3.4, 4.3, 4.5, 2.3,
   #0.5, 4.9, 3.5, 1.8, 1.9, 2.6, 4.3, 4.6, 5.2, 1.6, 2.8, 2.4, 2.8,
    #1.8, 3.6, 0.8, 5.1, 1.4, 3.2, 1, 6.3, 3.6, 3.6, 1.8, 0.9, 4.6,
     #2.5, 5.8, 0.6, 3.3, 3.2, 6.6, 2.6, 2.5, 1.5, 4.1, 1.7, 2.1, 1.5,
      #0.4, 4.8, 0.4, 1.5, 4.2, 3.3, 1.2, 8.1, 2.4, 2.8, 2.1, 6.3, 4.2,
       #1.3, 6, 1.3, 3.7, 2.5, 6.6, 2.7, 1.4, 2, 0.7, 4.3, 3.4, 4.3, 4,
        #4, 0.8, 2.3, 2.5, 5.4, 4.3, 0.5, 3.9, 2.2, 3.4, 1.3, 2.4, 4.7,
         #2, 1.3, 4.4, 2.9, 2.1, 2.5, 1.6, 2.3, 4.4, 1.9, 1.9, 1.7, 2,
          #4.2, 3.4, 3.9, 4.3, 1.3, 2.9, 2.2, 5.1, 2.3, 1.9, 2.9, 5.2,
           #3.4, 2.6, 2.4, 3.2, 1.3, 3.1, 5.1, 1.4, 4.2, 0.9, 1.3, 2.1,
            #2.6, 6.2, 1.6, 2.7, 1.7, 2.3, 3.3, 2.8, 1.2, 2.6, 1.5, 2,
             #2.8, 2.5, 2, 1.2, 2.2, 2.6, 2.5, 6, 1.9, 3, 3.8, 1.9,
              #3.2, 3.1, 1.8, 2.6, 1.9, 3.5, 3.7, 1.8, 2.2, 2, 1.3, 2,
               #1.1, 2.2, 3.1, 2.9, 1.3, 0.2, 3.9)

dados <- c(2.5, 0.9, 2.2, 1.9, 3.2, 4.5, 1.6, 1.7, 4.4, 1, 0.8, 4.3, 1.4, 1.9, 2.3, 0.8, 4.3, 2.6, 5.5, 0.3, 4.8, 2.6, 3, 2.2, 2.2, 1.8, 3.5, 2.8, 3.5, 3.2, 2, 1.8, 1.4, 6.6, 0.9, 2.1, 1.5, 4, 0.8, 0.8, 3.2, 2, 2.5, 3.6, 3.6, 2.3, 2.6, 3.7, 3, 1.1, 1.1, 1, 1.5, 1.6, 3, 0.9, 0.6, 2.2, 3.7, 1.5, 1.2, 2.2, 2.7, 2.7, 3.2, 3.5, 2.7, 3.3, 2.9, 1.6, 3, 4.2, 3.4, 5.7, 0.2, 4.4, 5.5, 1.7, 1.6, 1, 2.8, 1.5, 2.2, 1.8, 1.1, 4, 4.9, 1.6, 2.6, 2.3, 2.1, 2.5, 2.6, 1.5, 0.9, 1.1, 7.1, 5, 0.8, 0.7, 2.4, 4.3, 2.1, 4.5, 2.8, 4.4, 2.7, 2.5, 1.3, 6.6, 2.1, 2.4, 3.2, 1.1, 2.2, 1.7, 2.1, 4.2, 4.2, 0.9, 2, 1.9, 1.5, 4.6, 3.1, 4.1, 4.1, 3.9, 2.2, 2, 3.2, 1.4, 2.6, 5.1, 2.7, 5.4, 1.5, 2.9, 1.8, 4.6, 3.2, 0.9, 2.5, 3, 2.2, 2.2, 5.1, 4.3, 0.9, 0.5, 1.5, 1.7, 2.9, 1.3, 2, 1.5, 1.1, 2.6, 3.1, 2.6, 4.1, 2, 2.8, 2.9, 1.1, 1.9, 2.2, 2.5, 3.2, 1.7, 2.5, 3.8, 3.2, 1.6, 0.9, 1.5, 1.2, 2.5, 2.6, 2.4, 1.1, 6.1, 2.3, 1.4, 2.3, 2.6, 2.8, 3.3, 0.3, 5.3, 2.8, 2.1, 2.3, 1.1, 1.7, 2.1, 0.7, 1, 3.9, 2.4, 3.6, 0.8, 2.6, 0.9, 2.1, 4.5, 5.4, 3.8, 4.3, 2.1)

# 1. Fixar a semente e selecionar subamostra
set.seed(3379)  # Nota: Semente corrigida conforme solicitado
subamostra <- sample(dados, size = 168, replace = FALSE)

# Parâmetro da distribuição de Rayleigh
sigma <- 2.2

# 2. Calcular os limites das 5 classes equiprováveis sob H₀
qrayleigh <- function(p, sigma) {
  return(sigma * sqrt(-2 * log(1 - p)))
}

limites <- c(0, qrayleigh(c(0.2, 0.4, 0.6, 0.8), sigma), Inf)

# 3. Agrupar as observações nas classes definidas
classes <- cut(subamostra, breaks = limites, include.lowest = TRUE)
freq_observadas <- table(classes)

# 4. Calcular o valor-p do teste qui-quadrado
teste_qui <- chisq.test(freq_observadas)
valor_p <- teste_qui$p.value

cat("Valor-p do teste qui-quadrado:", valor_p, "\n")

# Determinar decisão baseada no valor-p
if (valor_p < 0.01) {
  decisao <- " Rejeitar H₀ aos n.s. de 1%, 5% e 10%."
} else if (valor_p < 0.05) {
  decisao <- " Rejeitar H₀ aos n.s. de 5% e 10% e não rejeitar H₀ ao n.s. de 1%."
} else if (valor_p < 0.10) {
  decisao <- " Rejeitar H₀ ao n.s. de 10% e não rejeitar H₀ aos n.s. de 1% e 5%."
} else {
  decisao <- "Não rejeitar H₀ aos n.s. de 1%, 5% e 10%."
}

cat("Decisão:", decisao)
