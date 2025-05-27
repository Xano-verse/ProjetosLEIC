from matplotlib import pyplot as plt
import subprocess, time, math
import numpy as np

# fazer pip install matplotlib no termianal/shell/CommandPrompt

# MUDAR O NUMERO DE INPUTS QUE QUEREM NA LINHA 15
# MUDAR O NOME DO VOSSO PROJETO NA LINHA 19
# MUDAR A COMPLEXIDADE DO VOSSO CODIGO NA LINHA 26

# de resto ta tudo feito por vcs só têm de correr este ficheiro na diretoria
# onde tem o vosso projeto e gerador (com nome gera.cpp)

# AJUSTAVEL
num_inputs = 50

generator_path = 'gera.cpp'
generator_exe = 'gera'
project_path = 'projeto.cpp'
project_exe = 'proj2'
input_file = 'input.in'

def calculate_comp(n, m, l):
    # retornar função de complexidade, por exemplo:
    # se temos O(n^4m^2) return n**4 * m**2
    return m * math.log2(n*l)

def generate_input_size():
    input_size = []
    for i in range(0, num_inputs):
        n = 50000+(i+1)*5000
        m = 100000+(i+1)*10000
        l = int(100+20*math.log(i+1))
        input_size.append([n, m, l])

    return input_size

def run(input_size):
    times = []

    # compilar o gerador
    #subprocess.run(["g++", generator_path, "-o", generator_exe])
    subprocess.run(["make"])

    # executar o gerador com n e m e l
    for size in input_size:
        n = size[0]
        m = size[1]
        l = size[2]

        # executar o gerador
        with open(input_file, "w") as file:
            program_args = [str(n), str(m), str(l), '1']
            subprocess.run(["./" + generator_exe] + program_args, stdout=file)

        # calcular tempo do ficheiro de input (tambem corre o programa)
        time = round(calculate_time(), 2)
        times.append(time)

    return times

def calculate_time():
    # compilar o projeto
    subprocess.run(["g++", project_path, "-o", project_exe])

    # abrir o ficheiro gerado
    with open("input.in", "r") as file:
        # tempo de inicio
        start_time = time.perf_counter()

        # correr o projeto
        result = subprocess.run(["./" + project_exe], stdin=file, capture_output=True, text=True)

        # tempo de fim
        end_time = time.perf_counter()

        output = result.stdout
        print('Output:', output)

    return end_time-start_time

def f(input_size):
    comp = []

    for size in input_size:
        comp.append(calculate_comp(size[0], size[1], size[2]))

    return comp

def main():
    # tamanho do input [n, m, l]
    input_size = generate_input_size()
    # calcular tempo de cada input
    times = run(input_size)
    # calcular f(n, l)
    comp = f(input_size)

    # Degree 1 para ser linear
    coefficients = np.polyfit(np.array(comp), np.array(times), 1)
    print(coefficients)
    slope, intercept = coefficients
    # equação da linha
    line = slope * np.array(comp) + intercept

    print('Complexidade:', comp)
    print('Time:', times)

    # desenhar os pontos
    plt.scatter(comp, times, color="blue")
    # desenhar a linha
    plt.plot(comp, line)
    plt.xlabel('f(n,m,l)')
    plt.ylabel('Time (s)')
    plt.title('Análise teórica')
    plt.show()

main()
