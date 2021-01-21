# Examen2021
Examen IN 203 du 21 Janvier 2021

Pensez à la fin de l'examen de nous envoyer soit le lien sur les sources (lien github, google drive, etc.) ou bien directement les sources, NON COMPRESSEES,
aux adresses usuelles.

# Introduction

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

# Suite de Syracuse

## Syracuse simple

L'exécution séquentielle donne les résultats suivants :   
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 0.405557 secondes.  

On utilise le package de OpenMP pour paralléliser avec le code  
`#pragma omp for`
 
 L'execution après parallélisation de boucle en mémoire partagée  avec 2 threads donne les résultats suivants :    
 Hauteur moyenne calculee : 7.90495e+07  
 Longueur moyenne calculee : 158.089  
 Temps mis par le calcul : 0.19598 secondes.  

L'execution après parallélisation de boucle en mémoire partagée  avec 4 threads donne les résultats suivants :    
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 0.197834 secondes.  

L'execution après parallélisation de boucle en mémoire partagée  avec 8 threads donne les résultats suivants :    
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 0.192508 secondes.  

Nous pouvons voir qu'au-dèla deux threads, l'accélération est très décevante. C'est parce que ma machine virtuelle a des performances limitées, elle a possède juste 2 coeurs de calculs.  

## Syracuse avec orbite

D'abord, on change le nb_echantillon à 1000000 pour mieux comparer avec Syracuse simple.

L'exécution séquentielle donne les résultats suivants :   
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 1.98104 secondes.  
Valeur la plus grand atteinte : 858555169576  

L'execution après parallélisation de boucle en mémoire partagée  avec 2 threads donne les résultats suivants :    
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 1.13311 secondes.  
Valeur la plus grand atteinte : 858555169576  


L'execution après parallélisation de boucle en mémoire partagée  avec 4 threads donne les résultats suivants :    
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 1.16254 secondes.  
Valeur la plus grand atteinte : 858555169576  


L'execution après parallélisation de boucle en mémoire partagée  avec 8 threads donne les résultats suivants :    
Hauteur moyenne calculee : 7.90495e+07  
Longueur moyenne calculee : 158.089  
Temps mis par le calcul : 1.17217 secondes.  
Valeur la plus grand atteinte : 858555169576  

Nous avons vu le même phénomène qu'avant, et je pense que cela est dû au problème de performances de ma machine virtuelle. Avec nb_echantillon à 1000000, nous avons également remarqué que ce programme prend presque 10 fois plus de temps que le programme précédent. La raison principale est, dans le second programme, on conserve les valeurs prises par les suites dans un tableau et Il faut beaucoup de temps pour sauvegarder ces données dans la mémoire.  

# Automate cellulaire

## Parallélisation OpenMP

L'exécution séquentielle donne les résultats suivants :   
Temps mis par le calcul : 1.34353 secondes.  
Temps mis pour constituer les images : 0.523534 secondes.

 L'execution après parallélisation de boucle en mémoire partagée  avec 2 threads donne les résultats suivants :    
 Temps mis par le calcul : 0.736882 secondes.  
 Temps mis pour constituer les images : 0.551243 secondes.  

L'execution après parallélisation de boucle en mémoire partagée  avec 4 threads donne les résultats suivants :    
Temps mis par le calcul : 0.752643 secondes.  
Temps mis pour constituer les images : 0.513487 secondes.


L'execution après parallélisation de boucle en mémoire partagée  avec 8 threads donne les résultats suivants :    
Temps mis par le calcul : 0.728492 secondes.  
Temps mis pour constituer les images : 0.530239 secondes.

Pour la partie calcul, il est cohérent avec les résultats obtenus auparavant. Au-dèla deux threads, l'accélération est très décevante. Mais pour la partie sauvegarde d'image, nous n'avons pas obtenu l'effet d'accélération. Je pense que c'est parce que le processus de sauvegarde d'image occupera les ressources de sortie publiques du système et que chaque thread doit attendre les uns les autres. En d'autres termes, le processus de sauvegarde d'image n'est pas parallélisable.  

## Parallélisation MPI
```
[ubuntu:18212] *** Process received signal ***
[ubuntu:18212] Signal: Segmentation fault (11)
[ubuntu:18212] Signal code: Address not mapped (1)
[ubuntu:18212] Failing at address: (nil)
[ubuntu:18212] [ 0] /lib/x86_64-linux-gnu/libpthread.so.0(+0x12980)[0x7f9275ee9980]
[ubuntu:18212] *** End of error message ***
--------------------------------------------------------------------------
mpirun noticed that process rank 4 with PID 0 on node ubuntu exited on signal 11 (Segmentation fault).
```

C'est un échec, je suis en train de le résoudre.
