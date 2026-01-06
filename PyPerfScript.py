from os import listdir, getcwd
from os.path import isfile, join
import matplotlib.pyplot as plt
import numpy as np

# -------------------------------
# Trouve les fichiers perfs
# -------------------------------
path = getcwd()
files = [f for f in listdir(path) if isfile(join(path, f))]

txtfiles = []
for elem in files:
    if elem.endswith(".txt") and elem.startswith("fichier") and "perf" in elem:
        txtfiles.append(elem)

print("Fichiers txt trouvés :", txtfiles)

# -------------------------------
# Parsing des fichiers perfs
# -------------------------------
data = {}

for file in txtfiles:
    with open(join(path, file), "r") as f:
        lines = [line.strip() for line in f.readlines()]

        algo = lines[0]
        word_count = int(lines[1])
        time_sec = float(lines[2])
        allocated_mem = int(lines[3])
        unallocated_mem = int(lines[4])
        peak_alloc = int(lines[5])

        result = {
            "mots": word_count,
            "temps": time_sec,
            "allouer": allocated_mem,
            "desallouer": unallocated_mem,
            "peak": peak_alloc
        }

        if algo not in data:
            data[algo] = []

        data[algo].append(result)

# -------------------------------
# Tri des données par total de mots
# -------------------------------
for algo in data:
    data[algo].sort(key=lambda x: x["mots"])

algos_ordonnes = sorted(data.keys())

# -------------------------------
# Plot 1: Comparaison de mémoire
# -------------------------------
plt.figure()

allocated = [data[a][0]["allouer"] for a in algos_ordonnes]
unallocated = [data[a][0]["desallouer"] for a in algos_ordonnes]
peak = [data[a][0]["peak"] for a in algos_ordonnes]

x = range(len(algos_ordonnes))
plt.bar([i - 0.25 for i in x], allocated, 0.25, label="Allouée")
plt.bar(x, unallocated, 0.25, label="Désallouée")
plt.bar([i + 0.25 for i in x], peak, 0.25, label="Peak")

plt.xticks(x, algos_ordonnes)
plt.xlabel("Algorithme")
plt.ylabel("Mémoire")
plt.title("Usage Mémoire par algorithme")
plt.legend()
plt.grid(axis="y")
plt.show()

# -------------------------------
# Plot 2: Temps d'exécution
# -------------------------------
plt.figure()

times = [np.mean([r["temps"] for r in data[a]]) for a in algos_ordonnes]

plt.bar(algos_ordonnes, times)
plt.xlabel("Algorithme")
plt.ylabel("Temps d'exécution (secondes)")
plt.title("Performances: Temps d'exécution par algorithme")
plt.ylim(0, max(times) * 1.2)
plt.grid(axis="y")
plt.show()

# -------------------------------
# Plot 3/4/5: Temps d'exécution de chacun des algorithmes
# -------------------------------
algo_name1 = algos_ordonnes[0]
algo_name2 = algos_ordonnes[1]
algo_name3 = algos_ordonnes[2]

plt.figure()

mots_1 = [r["mots"] for r in data[algo_name1]]
temp_1 = [r["temps"] for r in data[algo_name1]]

mots_2 = [r["mots"] for r in data[algo_name2]]
temp_2 = [r["temps"] for r in data[algo_name2]]

mots_3 = [r["mots"] for r in data[algo_name3]]
temp_3 = [r["temps"] for r in data[algo_name3]]

plt.plot(mots_1, temp_1, marker='o', linestyle='-', color='r', label=algo_name1)

plt.plot(mots_2, temp_2, marker='o', linestyle='-', color='g', label=algo_name2)

plt.plot(mots_3, temp_3, marker='o', linestyle='-', color='b', label=algo_name3)

plt.xlabel("Nombre de mots")
plt.ylabel("Temps d'exécution (s)")
plt.title(f"Temps d'exécution de chaques algorithmes")
plt.legend()
plt.grid(True)
plt.show()
