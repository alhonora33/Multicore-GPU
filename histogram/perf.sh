#!/bin/bash

# Fichier de sortie
OUTPUT_FILE="histogram_results_native.txt"

# Paramètres à varier
ARRAY_LENS=(32 64 128 256 512 1024 2048 4096 8182 16364 32728 66456 132912 263824 527648 1055296 2110592 4221184 8442368)

NB_BINS_LIST=(32 128 512 2048 4096)         # Valeurs pour --nb-bins
NB_REPEAT=20                  # Fixe
LOWER_BOUND=0                   # Fixe
UPPER_BOUND=10                  # Fixe

# Créer ou vider le fichier de sortie
> "$OUTPUT_FILE"

# Boucles pour varier les paramètres
for ARRAY_LEN in "${ARRAY_LENS[@]}"; do
    for NB_BINS in "${NB_BINS_LIST[@]}"; do
        echo "Running: --array-len=$ARRAY_LEN --nb-bins=$NB_BINS --nb-repeat=$NB_REPEAT"

        # Exécuter le programme
        ./histogram --array-len "$ARRAY_LEN" --nb-bins "$NB_BINS" --lower-bound "$LOWER_BOUND" --upper-bound "$UPPER_BOUND" --nb-repeat "$NB_REPEAT" >> "$OUTPUT_FILE"

        # Vérification des erreurs
        if [ $? -ne 0 ]; then
            echo "Error running: --array-len=$ARRAY_LEN --nb-bins=$NB_BINS --nb-repeat=$NB_REPEAT" >> "$OUTPUT_FILE"
        fi
    done
done

echo "Results saved in $OUTPUT_FILE"

