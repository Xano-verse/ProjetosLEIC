library(ggplot2)

# Read file
data <- read.csv("winequality-white-q5.csv")


ggplot(data, aes(x = sqrt(citric.acid), y = factor(quality))) +
	# outlier.shape = NA makes outliers invisible since we'll change them later
	geom_boxplot(fill = "lightblue", color = "black", outlier.shape = NA) +
	# outlier.size = 3 e outlier.shape = 16 e alpha = 0.1

	# geom_jitter adds each point spreaded with a width so it there isn't much overlap
	geom_jitter(width = 0.4, height = 0, alpha = 0.4, color = "blue") +

	# labels
	labs(title = "Wine Quality by sqrt(Citric Acid)",
       x = "sqrt(Citric Acid)",
       y = "Wine Quality") +
  theme_minimal()
  

