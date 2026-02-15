# Projet d'informatique CSC_42012 : Convex hulls

## Émile Chaurang, Alexandre Paresy

Projet effectué dans le cadre d'un cours donné en deuxième année à l'École Polytechnique. Celui-ci consiste en l'implémentation et l'analyse de deux algorithmes d'enveloppes convexes, ainsi que la réalisation de visualisations pour ceux-ci. Les algorithmes sont implémentés en C++ et les résultats sont visalisés à l'aide de la bibliothèque `matplotlib` en Python.

Pour modifier le nombre de points par dataset, la constante correspondante se trouve dans `convex_hull.h`.

Les différentes commandes d'exécution sont les suivantes :

Pour exécuter chaque algorithme, obtenir une comparaison des résultats dans `results.txt` et une visualisation des deux algorithmes sur chaque dataset : `make run`

Pour faire `NB_ITER` exécutions de chaque algorithme sur chacun des quatre datasets et obtenir les temps d'exécution moyens : `make perf`

Pour accéder à la visualisation en direct de l'exécution de l'algorithme de balayage : `make visu`

Pour nettoyer les fichiers : `make clean`

Les seules dépendances sont matplotlib et SFML pour les visualisations.