from time import time
import subprocess

# Path to the python script
script = "projeto.py"


# Create a list to store the execution times and the values of f(n,m,l)
times = []
f_nml = []

with open("testes.txt", "w") as out:
   # Teste the speed of the executable with all the tests
   for i in range(0, 224):

      # Read the parameters of the test
      with open(f"tests/test_{i:03d}.in", "r") as f:
         line = f.readline().split()
         n = line[0]
         m = line[1]
         t = line[2]
      
      # Start the timer
      start = time()
      
      # Run the executable with the input file
      with open(f"tests/test_{i:03d}.in", "r") as input_file:
         subprocess.run(['python3', script], stdin=input_file)
      
      # Stop the timer
      end = time()

      # Calculate the execution time
      execution_time = end - start
      
      # Write the results to the output file
      out.write(f"{n} {m} {t} {execution_time}\n")

      # Give feedback to the user
      print(f"Teste {i:03d} concluído. Tempo de execução: {execution_time:.4f} segundos")
