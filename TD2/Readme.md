# TP2 de WANG Yu
## Exercice à rendre
### 1. Question du cours
**(1) Un premier scénario où il n’y a pas d’interblocage:**

Dans le premier cas, le processus 2 recevra d'abord le message envoyé par le processus 0, puis, après avoir reçu ce message, il enverra un message au noyau 0. Enfin, le processus 1 envoie un message au processus 2, qui peut le recevoir sans problème.

**(2) Un deuxième scénario où il y a interblocage:**

Dans le second cas, avant de recevoir le message envoyé par le processus 0, le processus 2 recevra d'abord le message du processus 1. À ce stade, le processus 2 enverra un message au processus 0, mais le processus 0 attend toujours que le processus 2 reçoive le message avec succès. Il y a donc interblocage.  
En fait, il y a six situations où **O** représente que le programme s'exécute normalement, et **X** représent qu'il y a interblocage.

| 012 | 021 | 102 | 120 | 201 | 210 |
| --- | --- | --- | --- | --- | --- |
| **O** | **O** | **X** | **X** | **O** | **X** |

Puisque la probabilité de chaque situation est égale, la probabilité d‘avoir interblocage est 0,5.

### 2. Question du cours n°2
**(1)**  
**Loi d'Amdhal = n/(1+(n-1)f).**  
Ici, on a f est égal à 0.1, on a donc S = 10/(1+9/n).  
En considérant que n >> 1, S est égal à 10.

**(2)**  
Si on trace S(n) et on suppose S >= 9, c'est-à-dire 10/(1+9/n) >= 9, alors on a n >= 81.

**(3)**  
En utilisant la loi de Gustafon, on a s + np = 4 avec s = 0.1, donc np = 3.9.  
Si on double le nombre de données, on a donc Ss(n) = s + 2np = 0.1 + 2*3.9 = **7.9**.

### 3. Ensemble de mandelbrot
**(1)**  
**Avec la commande:**  
```cpp
make mandelbrot
```   
**Je constate que:**  
mpiexec -oversubscribe -np 8 ./mandelbrot.exe 
Temps de calcul d'un processus : 0.229605  
Temps de calcul d'un processus : 0.266108  
Temps de calcul d'un processus : 2.3885  
Temps de calcul d'un processus : 2.46982   
Temps de calcul d'un processus : 2.51548  
Temps de calcul d'un processus : 2.10748  
Temps de calcul d'un processus : 6.51303  
Temps de calcul d'un processus : 6.57672    
Temps total :6.9577s.  

**Cependant, lorsque j'exécute un programme avec un seul processus, le temps de calculer le mandabrot est**  
Temps calcul ensemble mandelbrot : 21.5763  

Par conséquence, speedup S(8) = ts/tp(8) = 21.5763/6.9577 = **3.09**  

Dans ce code, je répartis uniformément le nombre de lignes d'image en fonction du nombre total de processus. Après cela, j'utilise la fonction MPI_Gather pour obtenir les résultats de chaque processus. Bien que le nombre de pixels à calculer pour chaque processus soit le même, la quantité de calcul est différente, ce qui entraîne une grande différence dans le temps de calcul.

**(2)**  
**Avec la commande:**  
```cpp
make mandelbrot_M-E
```   
**Je constate que:**  
mpiexec -oversubscribe -np 8 ./mandelbrot_M-E.exe  
Temps total :3.62142s.  

Par conséquence, speedup S(8) = ts/tp(8) = 21.5763/3.62142 = **5.94**

**Conclusion: La stratégie maître–esclave évite le gaspillage de ressources causé par des threads à exécution rapide en attente d'autres threads.**

### 4. Produit matrice–vecteur
**(1)**  
Je ne sais pas comment calculer le produit après je partitionne la matrice par un découpage par bloc de colonnes.

**(2)**  
J'utilise les fonctions MPI_Scatter et MPI_Gather pour communiquer collective. Veuillez vérifier le code correspondant.
