library(ggplot2)
library(readxl)

dados <- read.csv("clima.csv")

# Select all columns but only the rows from March 2010
dados_marco_2010 <- dados[ substr(dados$Data, 1, 7) == "2010-03", ]

# Data frame com coluna de datas, mas apenas dias, sem as horas, e coluna poluicao dos dados de marco
datas_diarias <- data.frame(Data = substr(dados_marco_2010$Data, 1, 10), Poluição = dados_marco_2010$Poluição)

# Calcular mediana e juntar todos as linhas com o mesmo dia numa so linha
mediana_diaria <- aggregate(Poluição ~ Data, data = datas_diarias, median)

# Mudar o nome da coluna porque agora é a mediana da poluição
names(mediana_diaria)[2] <- "Mediana"

# Converter colunas das datas para POSIXct objects para o ggplot conseguir transformar numa escala para o eixo
dados_marco_2010$Data <- as.POSIXct(dados_marco_2010$Data, format = "%Y-%m-%d %H:%M:%S")
mediana_diaria$Data <- as.POSIXct(mediana_diaria$Data, format = "%Y-%m-%d")

ggplot() +
  geom_line(data = dados_marco_2010, aes(x = Data, y = Poluição, color = "Poluição horária"), group = 1) +
  geom_line(data = mediana_diaria, aes(x = Data, y = Mediana, color = "Mediana diária"), group = 1) +

  scale_color_manual(values = c("Poluição horária" = "blue", "Mediana diária" = "red")) +

  # Definir os intervalos da escala temporal dos x
  scale_x_datetime(
    date_breaks = "4 days",
    date_labels = "%d %b"
  ) +

  # Labels
  labs(
    title = "Variação Horária da Poluição - Março de 2010",
    subtitle = "Inclui mediana diária da poluição",
    x = "Data e Hora",
    y = "Nível de Poluição",
    color = "Legenda",  # título da legenda
  ) +
  theme_minimal()
