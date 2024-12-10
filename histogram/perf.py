import matplotlib.pyplot as plt
import pandas as pd

# Charger les données depuis un fichier texte
input_file = "histogram_results_native.txt"

# Lire les données en ignorant les lignes non conformes (si présentes)
data = []
with open(input_file, "r") as file:
    for line in file:
        try:
            if line.strip() and "array_len" not in line:
                data.append(line.strip().split(","))
        except ValueError:
            pass  # Ignorer les lignes mal formées

# Convertir les données en DataFrame
columns = ["array_len", "nb_bins", "nb_repeat", "rep", "timing", "check_status"]
df = pd.DataFrame(data, columns=columns)

# Convertir les colonnes nécessaires en float
df["array_len"] = df["array_len"].astype(int)
df["nb_bins"] = df["nb_bins"].astype(int)
df["timing"] = df["timing"].astype(float)

# Calculer la moyenne des timings pour chaque combinaison de array_len et nb_bins
averaged_df = df.groupby(["array_len", "nb_bins"])["timing"].mean().reset_index()

# Renommer la colonne des timings moyens
averaged_df.rename(columns={"timing": "avg_timing"}, inplace=True)

# Tracer les courbes de timing en fonction de array_len et nb_bins
for nb_bins in averaged_df["nb_bins"].unique():
    subset = averaged_df[averaged_df["nb_bins"] == nb_bins]
    plt.plot(
        subset["array_len"],
        subset["avg_timing"],
        marker="o",
        label=f"nb_bins={nb_bins}",
    )
# logger le plot
plt.xscale("log")


plt.title("Timing vs Array Length for Different Bin Counts")
plt.xlabel("Array Length")
plt.ylabel("Average Timing (seconds)")
plt.legend()
plt.grid()
# Enregistre le plot dans un fichier
output_file = "timing_plot_native.png"
plt.savefig(output_file)
print(f"Plot saved to {output_file}")

plt.show()
