import os
import statistics
import matplotlib.pyplot as plt

dir = "./"

first = []
second = []
third = []
fourth = []
fifth = []
sixth = []


for filename in os.listdir(dir):
    #print(filename[-4:-1] + filename[-1])
    if filename[-4:-1] + filename[-1] == ".txt":
        print(filename)

        with open(filename, "r", encoding='utf8') as file:
            for line in file:
               split_line = line.split('\t')
               #print(split_line[1])
               arr_size = split_line[0]
               stride = split_line[1]
               time = split_line[2]



               if stride == '1':
                   if arr_size in ['4096', '16384', '65536', '262144', '1048576', '4194304']:
                       print(line)

                       if arr_size == '4096':
                           first.append(time)
                       elif arr_size == '16384':
                           second.append(time)
                       elif arr_size == '65536':
                           third.append(time)
                       elif arr_size == '262144':
                           fourth.append(time)
                       elif arr_size == '1048576':
                           fifth.append(time)
                       elif arr_size == '4194304':
                           sixth.append(time)



print("size: 4096")
for i in first:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, first)))))

print("size: 16384")
for i in second:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, second)))))

print("size: 65536")
for i in third:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, third)))))

print("size: 262144")
for i in fourth:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, fourth)))))

print("size: 1048576")
for i in fifth:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, fifth)))))

print("size: 419304")
for i in sixth:
    print(i)
print("Mean time: " + str(statistics.mean(list(map(float, sixth)))))



strides = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192]
plt.plot(array_sizes, first, label="4096")

plt.plot(array_sizes, 
plt.plot(array_sizes, 
plt.plot(array_sizes, 
plt.plot(array_sizes, 
plt.plot(array_sizes, 
