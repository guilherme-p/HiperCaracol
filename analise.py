from random import randint

import csv

import os

import time

N = 1000

MIN_V = 100
MAX_V = 1000

RESULT_FILE = "results.csv"
results = []

for i in range(1, N+1):
    TEST_NAME = "test" + str(i)

    V = randint(MIN_V, MAX_V)

    largura = randint(5, 10)
    regularidade = randint(1, largura - 1)
    n_camadas = V // largura
    camada_de_corte = randint(1, n_camadas)
    fator_de_pesos = 1

    E = 2 * (V * regularidade)

    os.system("./gerador {} {} {} {} {} > {}".format(largura, regularidade, n_camadas, camada_de_corte, fator_de_pesos, TEST_NAME))

    start = time.time()
    os.system("./hiper < {} > {}".format(TEST_NAME, TEST_NAME + ".out"))
    end = time.time()

    time_taken = end - start

    results.append([time_taken, V**2 + V*(E**2) + V*E])


with open(RESULT_FILE, "w") as file:
    writer = csv.writer(file)
    writer.writerow(["Tempo", "V^2 + VE^2"])
    for result in results:
        writer.writerow(result)