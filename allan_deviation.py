import math
import matplotlib.pyplot as plt

array = []

file1 = open("probki2.txt", "r") #lokalizacja pliku z próbkami

lines = file1.readlines()

#przejście przez wszystkie linie w pliku
for line in lines:
    line = line.strip()
    if line[:2] == '00':
        line = line[2:]
    if line[:3] == '000':
        line = line[3:]
    line = float(line)
    line = line - 50000
    array.append(line*0.00000001)

#zamknięcie pliku
file1.close

steps = 1

for i in range(1, len(array)):
    if array[i-1] > 0 and array[i-1] > array[i]: 
        array[i] += steps * 0.001
        steps +=1
    else:
        array[i] += steps * 0.001
        
t = 0.01 #odstęp zbierania próbek sygnału 0,01s
n = 3 #początek przedziału o szerokości n=3
N = len(array) # liczba wszystkich próbek zebranych podczas pomiarów

file2 = open("probkiout2.txt", "a") #zapis wyników do pliku

for n in range(3, round(end)):
    sum = 0
    results = 0
    for i in range(0, N-2*n):
    #wzór na dewiacje Allana
        summ += pow(array[i+2*n] - 2 * array[i+n] + array[i],2)

    results = summ / (2 * pow(n,2) * pow(t,2) * (N - 2 * n) )
    results = math.sqrt(results)
    file2.write(str(results) + '\n')


file2.close
