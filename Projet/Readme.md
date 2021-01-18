# Rapport du projet "Simulation d'une colonisation d'une galaxie" - WANG Yu

## Informations sur ma machine

```
Architecture:        x86_64  
CPU op-mode(s):      32-bit, 64-bit  
Byte Order:          Little Endian  
CPU(s):              2  
On-line CPU(s) list: 0,1  
Thread(s) per core:  1  
Core(s) per socket:  1  
Socket(s):           2  
NUMA node(s):        1  
Vendor ID:           GenuineIntel  
CPU family:          6  
Model:               61  
Model name:          Intel(R) Core(TM) i5-5257U CPU @ 2.70GHz  
Stepping:            4  
CPU MHz:             2700.000  
BogoMIPS:            5400.00  
Hypervisor vendor:   VMware  
Virtualization type: full  
L1d cache:           32K  
L1i cache:           32K  
L2 cache:            256K  
L3 cache:            3072K  
NUMA node0 CPU(s):   0,1  
```

## Exécution en séquentiel

L'exécution séquentielle donne les résultats suivants :   
`CPU(ms) : calcul 20.437  affichage 2.132`

## Parallélisation de boucle en mémoire partagée

On utilise le package de OpenMP pour paralléliser la fonction mise_a_jour dans le fichier parametres.cpp, avec le code  
`#pragma omp for`

Dans cette boucle, la variable galaxie_previous est constant, qui ne se sert qu’a lire. Donc il n’existe pas de competition en lisant les donnes.  
Par contre, la variable galaxie_next est à écrire. Quand on écrit le nouvel état d’une planète, il y aura peut-être une collision, qui varie le résultat en écrivant dans diffèrent ordre. Mais cela correspond aussi le problème, car ce n’est pas réaliste que les actions soient tous fait ligne par ligne.

L'execution après parallélisation de boucle en mémoire partagée  avec 4 threads donne les résultats suivants :    
`CPU(ms) : calcul 2.268  affichage 2.016`

On voit que nous avons obtenu des gains considérables en temps de calcul avec un speedup `S(n) = 10`  
Dans le même temps, nous avons également remarqué que le temps utilisé pour le calcul et le temps utilisé pour l'affichage sont similaires.

## Recouvrement calcul / affichage en mémoire partagée

Nous pouvons utiliser le package `pthread` pour paralléliser l'affichage et le calcul. La stratégie de base est la suivante:

- Le premier fil montre la galaxie initiale  
- Pendant ce temps d'affichage, le deuxième thread calculera l'état de la prochaine galaxie  
- Lorsque les deux threads sont terminés, le premier thread affichera l'état mis à jour, tandis que le second thread continue à calculer.

Cette méthode peut fonctionner normalement, car l'affichage et le calcul ne liront que l'état actuel de la galaxie, mais ne le changeront pas. Ce n'est qu'une fois l'affichage terminé que le deuxième thread passera le nouvel état à g via la fonction de permutation.

## Parallélisation en mémoire distribuée

Cette fois, on utilise MPI avec plusieurs processus: processus 0 est un coordinateur (le root) et les autres processus sont des travailleurs. L'algorithme à mémoire distribuée travaille comme suit :

### Tout d'abord, nous initialisons chaque processus:

#### Pour le processus 0 :  
- Initialiser `SDL` et afficher l'image initiale.
- Diviser l'image entière en blocs `nbp-1` et envoyer-les au processus correspondant.

#### Pour les autres processus : 
- Recevoir le bloc à calculer.

### Après cela, chaque processus peut entrer dans la boucle (while (1)) jusqu'à ce que l'utilisateur ferme l'interface graphique (termine le programme). Dans la boucle:  

#### Pour le processus 0 :  
- Divises les `cellules fantômes` requises et envoyer-les au processus correspondant. (Ici, nous envoyons d'abord la ligne sous le bloc de calcul correspondant en tant que `cellules fantômes` pour le processus 1, ... , le processus nbp-2. Après cela, nous envoyons la ligne sur le bloc de calcul correspondant en tant que `cellules fantômes` pour le processus 2, ... , le processus nbp-1.)
- Recevoir le résultat du calcul de chaque processus et metter-les à jour dans la galaxie.
- Afficher l'image de la galaxie mise à jour.

#### Pour les autres processus : 
- Recevoir les `cellules fantômes` requises. (Une ligne pour le processus 1 et le processus nbp-1, deux lignes pour les autres processus).
- Combiner les `cellules fantômes`  reçues avec la zone à calculer et mettre à jour
- Diviser le bloc requi du résultat de la mise à jour et envoyer-le au processus 0

L'execution après parallélisation en mémoire distribuée avec 5 processus donne :  
`CPU(ms) : calcul 19.873  affichage 2.325`

Nous avons remarqué que nous n'avons pas gagné le temps de calcul (speedup ≈ 1) ni même le temps d'affichage (speedup ≈ 1).  
Je pense qu'il y a quatre raisons principales à cela:  
- Ma machine virtuelle n'a que 2 cœurs.
- L'échange de messages dans MPI est très coûteux.
- Un grand nombre de transmissions bloquantes fera que le temps d'attente deviendra le temps d'attente principal.
- L'existence de régions séquentielles non parallèles rend également difficile l'optimisation du temps de calcul et d'affichage.

En effet, il ne faut pas que les sous-taches soient trop petites sinon il y aura beaucoup d'echanges et on perd en efficacité.
