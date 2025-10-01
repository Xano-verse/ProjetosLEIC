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

