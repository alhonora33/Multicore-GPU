#IT390 CISD – Programmation des architectures multicœurs et accélérateurs
Devoir noté
2024 – 2025

0. Introduction
   Le but de ce devoir est de mettre en application les techniques de programmation étudiées en cours sur des exemples concrets, et de mesurer leur influence sur les performances et la consommation d'énergie. Le devoir est à réaliser par groupes de deux étudiants. La liste des binômes est disponible ici. Les techniques employées et les résultats obtenus seront présentés et commentés et à l'occasion d'une soutenance de 10 minutes suivie de 5 minutes de questions.

Dates importantes
Date limite d'envoi des codes : le mardi 10 décembre à midi.
Date de la soutenance : le vendredi 13 décembre de 13h00 à 18h00.

1. Programme "Stencil"
   Le premier programme à explorer est un programme appliquant un calcul de type stencil sur les valeurs d'un tableau à deux dimensions. Ce calcul est appliqué itérativement selon un nombre sélectionné d'itérations.

Les calculs de types stencil sont fréquemment utilisés dans les applications de simulations scientifiques pour simuler de manière approchée et par des méthodes discrétisées, des phénomènes dont la modélisation fait intervenir des équations aux dérivées partielles. Le principe, à chaque itération, est d'appliquer simultanément un même motif de calcul à chaque valeur du tableau faisant intervenir cette valeur proprement dite et son voisinage, et de mettre à jour chacune des cases du tableau avec le résultat du calcul correspondant.

Code source
Le programme initial est disponible dans le fichier stencil.tgz . L'archive contient: - stencil.c: le code source du programme; - Makefile: le makefile pour compiler le programme avec la commande make; - disp_mesh.py: un programme en python permettant d'afficher des tableaux 2D résultant du programme stencil, sous forme textuelle ou graphique, pour aider à la vérification des résultats et au débogage.

Dans le programme initial, la fonction naive_stencil_func() présente une implémentation naïve du calcul du stencil.

Travail demandé
Pour le programme stencil.c, vous utiliserez comme machine de référence l'un des nœuds de calcul miriel de la plateforme PlaFRIM. Pensez à vous assurer que vous êtes seuls à utiliser votre nœud de calcul pour éviter de fausser les mesures de performance.

Votre travail sera: - d'implémenter de nouvelles versions améliorées de la fonction naive_stencil_func(); - de les brancher dans la fonction run() qui réalise les itérations du calcul; - de valider que le calcul est correct indépendamment de la largeur et de la hauteur du tableau, et du nombre d'itérations choisies en argument de ligne de commande (on supposera que la largeur et la hauteur du tableau sont au moins égales à celles du tableau des coefficients du stencil); - de mesurer et comparer les performances et la consommation d'énergie obtenues de chaque version par rapport à la version naïve de référence.

Note: Il est important de conserver les différentes versions que vous allez réaliser, de manière à les comparer facilement, et à pouvoir les présenter facilement lors de la soutenance.

Versions à réaliser
Il est demandé de réaliser a minima les versions suivantes: - Une version utilisant les techniques de programmation vectorielle via le jeu d'instruction SIMD "Intel AVX2" disponible sur les nœuds miriel de la machine PlaFRIM. - Une version utilisant les techniques de programmation parallèle à base de tâches via le support d'exécution StarPU. - Une version combinant les deux précédentes, à savoir la programmation vectorielle et la programmation parallèle à base de tâches.

Vous êtes par ailleurs libres de proposer des versions supplémentaires qui vous paraitront pertinentes.

2. Programme "Histogramme"
   Le second programme à explorer est un programme calculant un histogramme des valeurs d'un tableau, c'est-à-dire pour un certain nombre d'intervalles, le nombre de valeur du tableau comprises dans chaque intervalle. Le calcul d'histogramme est couramment utilisé en statistiques pour étudier la distribution d'un ensemble de données. Il est également largement utilisé en photographie et vidéo numérique.

Code source
Le programme initial est disponible dans le fichier histogram.tgz disponible sur Moodle. L'archive contient: - histogram.c: le code source du programme; - Makefile: le makefile pour compiler le programme avec la commande make; - np_histogram.py: un programme en Python permettant de calculer l'histogramme avec la commande correspondante du module NumPY, pour comparer à vos résultats; - plot_histogram.py: en Python permettant d'afficher graphiquement l'histogramme obtenu, pour aider à la mise au point et au débogage.

Dans le programme initial, la fonction naive_compute_histogram() présente une implémentation naïve du calcul de l'histogramme.

Travail demandé
Pour le programme histogram.c, vous utiliserez comme machine de référence l'une des machines équipée d'un GPU du CREMI. Pensez à vous assurer que vous êtes seuls à utiliser la machine pour éviter de fausser les mesures de performance.

Votre travail sera: - d'implémenter de nouvelles versions améliorées de la fonction naive_compute_histogram(); - de les brancher dans la fonction run() qui réalise les itérations du calcul; - de valider que le calcul est correct indépendamment de la longueur du tableau choisie en argument de ligne de commande; - de mesurer et comparer les performances obtenues de chaque version par rapport à la version naïve de référence. Notez que pour cette partie, vous devez faire uniquement l'étude des performances (LIKWID n'est pas installé au CREMI).

Note: Il est important de conserver les différentes versions que vous allez réaliser, de manière à les comparer facilement, et à pouvoir les présenter facilement lors de la soutenance.

Versions à réaliser
Il est demandé de réaliser a minima les versions suivantes: - Une version OpenMP basique - Une version utilisant un accélérateur de type GPU, programmée avec l'environnement CUDA.

Vous êtes par ailleurs libres de proposer des versions supplémentaires qui vous paraitront pertinentes.

