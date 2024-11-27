# IT390 CISD – Programmation des architectures multicœurs et accélérateurs

### Devoir noté 2024 – 2025

## 0. Introduction

Le but de ce devoir est de mettre en application les techniques de programmation étudiées en cours sur des exemples concrets, et de mesurer leur influence sur les performances et la consommation d'énergie.  
Le devoir est à réaliser par **groupes de deux étudiants**. La liste des binômes est disponible [ici](#).  
Les techniques employées et les résultats obtenus seront présentés et commentés lors d’une **soutenance de 10 minutes**, suivie de **5 minutes de questions**.

### Dates importantes

- **Date limite d'envoi des codes** : le mardi 10 décembre à midi.
- **Date de la soutenance** : le vendredi 13 décembre de 13h00 à 18h00.

---

## 1. Programme "Stencil"

### Description

Le premier programme explore un calcul de type **stencil** appliqué sur un tableau 2D. Ce type de calcul est fréquemment utilisé dans les simulations scientifiques basées sur des équations aux dérivées partielles.

À chaque itération, un motif de calcul est appliqué à chaque valeur du tableau en tenant compte de cette valeur et de son voisinage. Le tableau est ensuite mis à jour avec les résultats du calcul.

### Code source

Le programme initial est disponible dans l’archive `stencil.tgz`, qui contient :

- **`stencil.c`** : le code source du programme ;
- **`Makefile`** : pour compiler le programme avec la commande `make` ;
- **`disp_mesh.py`** : un programme Python permettant d’afficher des tableaux 2D, de manière textuelle ou graphique.

Dans le programme initial, la fonction **`naive_stencil_func()`** propose une implémentation naïve du calcul stencil.

### Travail demandé

#### Objectifs

Pour **`stencil.c`**, vous utiliserez comme machine de référence un des nœuds de calcul **Miriel** de la plateforme PlaFRIM. Assurez-vous d’être seuls à utiliser le nœud de calcul pour éviter de fausser les mesures de performance.

Votre travail consistera à :

1. Implémenter de nouvelles versions améliorées de la fonction **`naive_stencil_func()`**.
2. Les intégrer dans la fonction **`run()`** qui réalise les itérations.
3. Valider le calcul indépendamment de la largeur, hauteur du tableau et du nombre d’itérations (assurez-vous que les dimensions du tableau soient compatibles avec les coefficients du stencil).
4. Mesurer et comparer les performances et la consommation d’énergie pour chaque version par rapport à la version naïve de référence.

#### Versions à réaliser

Vous devez implémenter **au minimum** les versions suivantes :

1. **Programmation vectorielle** avec les instructions SIMD **Intel AVX2** (disponibles sur les nœuds Miriel).
2. **Programmation parallèle à base de tâches** avec le support d'exécution **StarPU**.
3. Une **version combinée** utilisant à la fois la programmation vectorielle et la programmation parallèle.

Vous pouvez également proposer des versions supplémentaires si vous les jugez pertinentes.

---

## 2. Programme "Histogramme"

### Description

Le second programme calcule un **histogramme** des valeurs d’un tableau : pour un nombre donné d’intervalles, il compte combien de valeurs du tableau se trouvent dans chaque intervalle.  
Les histogrammes sont largement utilisés en statistiques, photographie et vidéo numérique.

### Code source

Le programme initial est disponible dans l’archive `histogram.tgz`, qui contient :

- **`histogram.c`** : le code source du programme ;
- **`Makefile`** : pour compiler le programme avec la commande `make` ;
- **`np_histogram.py`** : un programme Python utilisant NumPy pour calculer un histogramme (permet la vérification des résultats) ;
- **`plot_histogram.py`** : un script Python permettant d’afficher graphiquement l’histogramme obtenu.

Dans le programme initial, la fonction **`naive_compute_histogram()`** propose une implémentation naïve du calcul.

### Travail demandé

#### Objectifs

Pour **`histogram.c`**, vous utiliserez comme machine de référence une machine équipée d’un **GPU** du CREMI. Assurez-vous d’être seuls à utiliser la machine pour éviter de fausser les mesures de performance.

Votre travail consistera à :

1. Implémenter de nouvelles versions améliorées de la fonction **`naive_compute_histogram()`**.
2. Les intégrer dans la fonction **`run()`**.
3. Valider le calcul indépendamment de la longueur du tableau (spécifiée en argument de la ligne de commande).
4. Mesurer et comparer les performances pour chaque version par rapport à la version naïve de référence.

⚠️ **Remarque** : Pour cette partie, l’étude est limitée aux performances, car **LIKWID n’est pas installé** au CREMI.

#### Versions à réaliser

Vous devez implémenter **au minimum** les versions suivantes :

1. Une **version OpenMP basique**.
2. Une version utilisant un **GPU avec CUDA**.

---

## Notes supplémentaires

- Conservez toutes les versions que vous implémentez pour les comparer et les présenter lors de la soutenance.
- Respectez le format demandé pour l’envoi du code et préparez une présentation claire des résultats.
