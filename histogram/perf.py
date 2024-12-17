import os

import matplotlib.pyplot as plt
import pandas as pd

# Obtenir la liste des fichiers de résultats dans le répertoire courant
result_files = [
    file
    for file in os.listdir(".")
    if file.startswith("histogram_") and file.endswith("_results.txt")
]

# Créer un dictionnaire pour stocker les DataFrames par fichier
dataframes = {}

# Colonnes attendues dans les fichiers de résultats
columns = ["array_len", "nb_bins", "nb_repeat", "rep", "timing", "check_status"]

# Parcourir les fichiers et charger les données
for input_file in result_files:
    data = []
    with open(input_file, "r") as file:
        for line in file:
            try:
                if line.strip() and "array_len" not in line:
                    data.append(line.strip().split(","))
            except ValueError:
                pass  # Ignorer les lignes mal formées

    # Créer un DataFrame pour le fichier courant
    df = pd.DataFrame(data, columns=columns)

    # Convertir les colonnes nécessaires en types appropriés
    df["array_len"] = df["array_len"].astype(int)
    df["nb_bins"] = df["nb_bins"].astype(int)
    df["timing"] = df["timing"].astype(float)

    # Calculer la moyenne des timings pour chaque combinaison de array_len et nb_bins
    averaged_df = df.groupby(["array_len", "nb_bins"])["timing"].mean().reset_index()
    averaged_df.rename(columns={"timing": "avg_timing"}, inplace=True)

    # Ajouter le DataFrame au dictionnaire
    dataframes[input_file] = averaged_df

# Tracer un graphique pour chaque valeur de nb_bins
nb_bins_values = [20]
for nb_bins in nb_bins_values:
    plt.figure()
    for input_file, averaged_df in dataframes.items():
        subset = averaged_df[averaged_df["nb_bins"] == nb_bins]
        if not subset.empty:
            plt.plot(
                subset["array_len"],
                subset["avg_timing"],
                marker="o",
                label=input_file.replace("_results.txt", ""),
            )

    plt.xscale("log")
    # plt.yscale("log")
    plt.title(f"Timing for nb_bins = {nb_bins}")
    plt.xlabel("Array Length")
    plt.ylabel("Average Timing (seconds)")
    plt.legend()
    plt.grid()
    output_file = f"nb_bins_{nb_bins}_plot.png"
    plt.savefig(output_file)
    print(f"Plot saved to {output_file}")
    plt.show()
    plt.close()
