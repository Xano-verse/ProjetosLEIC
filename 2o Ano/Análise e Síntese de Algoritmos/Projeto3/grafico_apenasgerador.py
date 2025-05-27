import subprocess
import sys
import time
import matplotlib.pyplot as plt
import random

def generate_input(num_factories, num_countries, num_children):
    factories = []
    countries = []
    requests = []

    # Create factories
    for i in range(1, num_factories + 1):
        country_id = (i % num_countries) + 1
        stock = random.randint(num_factories, num_factories*2)
        factories.append(f"{i} {country_id} {stock}")

    # Create countries
    for i in range(1, num_countries + 1):
        max_exports = random.randint(num_countries//3, num_countries//2) 
        min_presents = random.randint(num_countries//10, num_countries//5)
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


print(generate_input(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])))
