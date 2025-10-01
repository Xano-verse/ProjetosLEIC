library(readxl)
library(ggplot2)

data <- read_excel("wine_prod_EU.xlsx")
clean_data <- data[data$Category != "-" & data$`Product Group` != "Non-Vinified", ]
data_2019 <- clean_data[clean_data$Year == 2019, ]

organized_countries <- data_2019
organized_countries$`Member State`[organized_countries$`Member State` != "France" &
								   organized_countries$`Member State` != "Italy" &
								   organized_countries$`Member State` != "Spain"] <- "Others"

organized_data <- aggregate(`Opening Stock` ~ `Member State` + Category,
							data = organized_countries,
							FUN = sum)
organized_data$Category <- factor(organized_data$Category,
								  levels = c("Varietal", "P.D.O. wines", "P.G.I. wines", "Other wines"))
organized_data$`Member State` <- factor(organized_data$`Member State`,
                                        levels = c("France", "Italy", "Spain", "Others"))
ggplot(organized_data, aes(x = Category, y = `Opening Stock`, fill = `Member State`)) +
	geom_bar(stat = "identity", position = "dodge") +
	labs(title = "Opening Stock of Wine by Category across Countries in 2019",
		 x = "Category",
		 y = "Opening Stock (x10³ hL)",
		 fill = "Countries") +
theme_minimal() 
