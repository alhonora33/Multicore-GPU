#!/bin/bash

# Paramètres à varier
ARRAY_LENS=(132912 263824 527648 1055296 2110592 4221184 8442368 16884736 33769472 67538944 135077888)

NB_BINS_LIST=(20)         # Valeurs pour --nb-bins
NB_REPEAT=10                    # Fixe
LOWER_BOUND=0                   # Fixe
UPPER_BOUND=10                  # Fixe

EXECUTABLES=(
  histogram_cuda_tmp
  histogram_cuda_shared
  histogram_cuda
)

# Boucles pour varier les paramètres
for EXECUTABLE in "${EXECUTABLES[@]}"; do
  # Générer le nom de fichier de sortie
  OUTPUT_FILE="${EXECUTABLE}_results.txt"
  > "$OUTPUT_FILE"
  for ARRAY_LEN in "${ARRAY_LENS[@]}"; do
    for NB_BINS in "${NB_BINS_LIST[@]}"; do
      echo "Running: $EXECUTABLE --array-len=$ARRAY_LEN --nb-bins=$NB_BINS --nb-repeat=$NB_REPEAT"

      # Exécuter le programme
      ./$EXECUTABLE --array-len "$ARRAY_LEN" --nb-bins "$NB_BINS" --lower-bound "$LOWER_BOUND" --upper-bound "$UPPER_BOUND" --nb-repeat "$NB_REPEAT" >> "$OUTPUT_FILE"

      # Vérification des erreurs
      if [ $? -ne 0 ]; then
        echo "Error running: $EXECUTABLE --array-len=$ARRAY_LEN --nb-bins=$NB_BINS --nb-repeat=$NB_REPEAT" >> "$OUTPUT_FILE"
      fi
    done
  done

done

echo "Results saved in $OUTPUT_FILE"

