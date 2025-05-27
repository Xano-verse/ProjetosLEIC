import subprocess
import time
import matplotlib.pyplot as plt

def simulate_execution(input_data):
    # Execute your project and measure time
    start_time = time.time()
    result = subprocess.run(
        ["python3", "projeto.py"],  # Replace with your project's filename
        input=input_data,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    end_time = time.time()

    # Measure execution time and return
    execution_time = end_time - start_time
    return execution_time, result.stdout.strip()  # Output for debugging

def generate_input(num_factories, num_countries, num_children):
    factories = []
    countries = []
    requests = []

    # Create factories
    for i in range(1, num_factories + 1):
        country_id = (i % num_countries) + 1
        stock = 10
        factories.append(f"{i} {country_id} {stock}")

    # Create countries
    for i in range(1, num_countries + 1):
        max_exports = 15
        min_presents = 5
        countries.append(f"{i} {max_exports} {min_presents}")

    # Create requests
    for i in range(1, num_children + 1):
        country_id = (i % num_countries) + 1
        num_requests = min(5, num_factories)
        factories_ids = list(range(1, num_requests + 1))
        requests.append(f"{i} {country_id} " + " ".join(map(str, factories_ids)))

    input_data = f"{num_factories} {num_countries} {num_children}\n"
    input_data += "\n".join(factories) + "\n"
    input_data += "\n".join(countries) + "\n"
    input_data += "\n".join(requests)
    return input_data

# Run experiments
results = []
increments = 10  # Increment size for n, m
initial_values = (10, 3)  # Initial values for n and m
num_tests = 20
k = 5  # Proportionality constant for t = k * n

for i in range(num_tests):
    n = initial_values[0] + i * increments
    m = initial_values[1] + i * increments
    t = k * n  # Ensure t grows proportionally to n
    #complexity = n * t + m
    complexity = t*n

    # Generate input
    input_data = generate_input(n, m, t)

    # Measure execution
    exec_time, output = simulate_execution(input_data)
    results.append((n, m, t, complexity, exec_time))

# Sort and plot
results.sort(key=lambda x: x[3])
complexities, times = zip(*[(r[3], r[4]) for r in results])

# Plot
plt.figure(figsize=(10, 6))
plt.plot(complexities, times, marker="o", linestyle="-", color="b") #label="Execution Time")
plt.xlabel("Número de variáveis n*t", fontsize=20)
plt.ylabel("Tempo (s)", fontsize=20)
plt.title("")
plt.grid()
plt.legend()
plt.show()

# Print table
print(f"{'Factories':<10}{'Countries':<10}{'Children':<10}{'Complexity':<15}{'Execution Time (s)'}")
for r in results:
    print(f"{r[0]:<10}{r[1]:<10}{r[2]:<10}{r[3]:<15}{r[4]:.4f}")
