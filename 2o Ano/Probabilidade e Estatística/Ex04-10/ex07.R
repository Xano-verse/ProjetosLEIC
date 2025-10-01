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


