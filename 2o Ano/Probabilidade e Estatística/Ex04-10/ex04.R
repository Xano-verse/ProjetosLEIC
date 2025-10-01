
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



