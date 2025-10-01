library(ggplot2)
library(readxl)
dados <- read.csv("clima.csv")
dados_marco_2010 <- dados[ substr(dados$Data, 1, 7) == "2010-03", ]
datas_diarias <- data.frame(Data = substr(dados_marco_2010$Data, 1, 10), Poluição = dados_marco_2010$Poluição)
mediana_diaria <- aggregate(Poluição ~ Data, data = datas_diarias, median)
names(mediana_diaria)[2] <- "Mediana"
dados_marco_2010$Data <- as.POSIXct(dados_marco_2010$Data, format = "%Y-%m-%d %H:%M:%S")
mediana_diaria$Data <- as.POSIXct(mediana_diaria$Data, format = "%Y-%m-%d")
ggplot() +
  geom_line(data = dados_marco_2010, aes(x = Data, y = Poluição, color = "Poluição horária"), group = 1) +
  geom_line(data = mediana_diaria, aes(x = Data, y = Mediana, color = "Mediana diária"), group = 1) +
  scale_color_manual(values = c("Poluição horária" = "blue", "Mediana diária" = "red")) +
  scale_x_datetime(
    date_breaks = "4 days",
    date_labels = "%d %b"
  ) +
  labs(
    title = "Variação Horária da Poluição - Março de 2010",
    subtitle = "Inclui mediana diária da poluição",
    x = "Data e Hora",
    y = "Nível de Poluição",
    color = "Legenda",  # título da legenda
  ) +
  theme_minimal()
