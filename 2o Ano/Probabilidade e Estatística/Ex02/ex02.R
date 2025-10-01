library(readxl)
library(ggplot2)
#library(openxlsx)

# Read file
data <- read_excel("wine_prod_EU.xlsx")

# Leaving , ] as blank selects all columns
# Manter os dados em que Category nao e vazia e Product Group nao e Non-Vinified 
clean_data <- data[data$Category != "-" & data$`Product Group` != "Non-Vinified", ]
#write.xlsx(clean_data, "clean-data.xlsx")

# Selecionar apenas o ano 2019
data_2019 <- clean_data[clean_data$Year == 2019, ]
#write.xlsx(data_2019, "data-2019.xlsx")


organized_countries <- data_2019
# Selecionamos os paises que nao sao nem franca nem italia nem espanha e declaramos o seu nome como Others
organized_countries$`Member State`[organized_countries$`Member State` != "France" &
								   organized_countries$`Member State` != "Italy" &
								   organized_countries$`Member State` != "Spain"] <- "Others"


# E o mesmo que fazer este loop aqui (roughtly, n testei se este loop funcionava 100%)
#for (x in data_2019$`Member State`) {
	#country <- data_2019$`Member State`[x]

	#if country != "France" & country != "Italy" & country != "Spain" {
		#organized_countries$`Member State`[x] <- "Others"
	#}
#}

# Agregar todas as instancias de cada pais numa só, somando os opening stocks por categoria
organized_data <- aggregate(`Opening Stock` ~ `Member State` + Category,
							data = organized_countries,
							FUN = sum)



# Ordenar categorias de vinho por uma ordem que faca mais sentido (so uso as categorias que sei que estao presentes)
organized_data$Category <- factor(organized_data$Category,
								  levels = c("Varietal", "P.D.O. wines", "P.G.I. wines", "Other wines"))


# Ordenar paises para o Others ser o ultimo 
organized_data$`Member State` <- factor(organized_data$`Member State`,
                                        levels = c("France", "Italy", "Spain", "Others"))


# Fazer grafico
ggplot(organized_data, aes(x = Category, y = `Opening Stock`, fill = `Member State`)) +
	# Grafico de barras
	# stat identity diz que os dados ja estao agregados
	geom_bar(stat = "identity", position = "dodge") +

	# Labels
	labs(title = "Opening Stock of Wine by Category across Countries in 2019",
		 x = "Category",
		 y = "Opening Stock (x10³ hL)",
		 fill = "Countries") +
theme_minimal() 
