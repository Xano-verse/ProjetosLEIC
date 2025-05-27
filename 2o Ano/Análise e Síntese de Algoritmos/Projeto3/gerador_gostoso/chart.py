import matplotlib.pyplot as plt
import numpy as np

# Create a list to store the execution times and the values of f(n,m,l)
times = []
f_nml = []

# Read data from file
with open("testes.txt", "r") as out:
    for i in range(0, 224):
        line = out.readline()
        line = line.split()
        n = int(line[0])
        m = int(line[1])
        t = int(line[2])
        exec_time = float(line[3])

        f_nml.append(n * t)
        times.append(exec_time)

# Convert lists to numpy arrays for processing
f_nml = np.array(f_nml)
times = np.array(times)

# Adjust a curve of degree 3
degree = 3
coef = np.polyfit(f_nml, times, degree)
poly_fn = np.poly1d(coef)

# Calculate residuals (difference between actual and predicted values)
predicted_times = poly_fn(f_nml)
residuals = times - predicted_times

# Define threshold for outliers (e.g., 2 standard deviations)
threshold = 2 * np.std(residuals)
outlier_indices = np.abs(residuals) > threshold

# Separate outliers and inliers
outliers_f_nml = f_nml[outlier_indices]
outliers_times = times[outlier_indices]
inliers_f_nml = f_nml[~outlier_indices]
inliers_times = times[~outlier_indices]


coef = np.polyfit(inliers_f_nml, inliers_times, degree)
poly_fn = np.poly1d(coef)

# Calculate residuals (difference between actual and predicted values)
predicted_times = poly_fn(inliers_f_nml)
residuals = inliers_times - predicted_times

# Define threshold for outliers (e.g., 2 standard deviations)
threshold = 2 * np.std(residuals)
outlier_indices = np.abs(residuals) > threshold

# Separate outliers and inliers
inliers_f_nml = inliers_f_nml[~outlier_indices]
inliers_times = inliers_times[~outlier_indices]

# Generate smooth points for the curve
x_curve = np.linspace(inliers_f_nml.min(), inliers_f_nml.max(), 800)
y_curve = poly_fn(x_curve)

# Plot inliers
plt.scatter(inliers_f_nml, inliers_times, alpha=0.5, color="blue")

# Plot outliers
#plt.scatter(outliers_f_nml, outliers_times, alpha=0.5, color="red", label="Outliers")

# Plot the curve
plt.plot(x_curve, y_curve, '--', color="red")

# Labels and legend
plt.xlabel("f(n,m,t)")
plt.ylabel("Time(s)")
plt.legend()
plt.show()
