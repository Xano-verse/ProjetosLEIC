import subprocess


def generate_test(n, m, t, max_factory_capacity, variance, max_requests, path):
   with open(path, 'w') as outfile:
      subprocess.run(
         ['python3', 'gera3.py', str(n), str(m), str(t), str(variance), str(max_factory_capacity), str(max_requests)],
         stdout=outfile,  # Redirect the output to the file
         stderr=subprocess.PIPE,  # Capture errors (if necessary)
         text=True  # Treat the data as text
      )

x = 0
variance = 0
for i in range(0, 14):
   n = 100 + 100*i
   for j in range(0, 1):
      m = 40
      for k in range(0, 16):
         t = 100 + 125*k
         max_factory_capacity = 5
         max_requests = n//20
         generate_test(n, m, t, max_factory_capacity, variance, max_requests, f"tests/test_{x:03d}.in")
         x += 1
         print(x)