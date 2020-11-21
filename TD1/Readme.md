##Exercice1: Un hello world parallele  
**Q1: Je constate que:**

Bonjour, je suis la tache n° 8 sur 16 taches.  
Bonjour, je suis la tache n° 9 sur 16 taches.  
Bonjour, je suis la tache n° 0 sur 16 taches.  
Bonjour, je suis la tache n° 11 sur 16 taches.  
Bonjour, je suis la tache n° 1 sur 16 taches.  
Bonjour, je suis la tache n° 10 sur 16 taches.  
Bonjour, je suis la tache n° 13 sur 16 taches.  
Bonjour, je suis la tache n° 14 sur 16 taches.  
Bonjour, je suis la tache n° 15 sur 16 taches.  
Bonjour, je suis la tache n° 4 sur 16 taches.  
Bonjour, je suis la tache n° 6 sur 16 taches.  
Bonjour, je suis la tache n° 7 sur 16 taches.  
Bonjour, je suis la tache n° 3 sur 16 taches.  
Bonjour, je suis la tache n° 12 sur 16 taches.  
Bonjour, je suis la tache n° 2 sur 16 taches.  
Bonjour, je suis la tache n° 5 sur 16 taches.  

**Q2: Le nom du fichier txt est nommé d'après le numéro de thread et correspond au thread correspondant. Le contenu est le résultat de l'exécution du thread. De cette façon, nous savons exactement ce que fait chaque thread.**  

##Exercice2: Envoi bloquant et non bloquant  

**Q1:**
**La première version est plus sûr, car elle attendra que la transmission réussie avant d'exécuter myvar = myvar + 2;**  

**Par contre, la deuxième version  exécutera myvar = myvar + 2 immédiatement après l'exécution de MPI_ISend.En d'autre termes, il n'attendra pas la transmission réussie de MPI_ISend. Cela entraînera une concurrence pour la valeur de myvar.**  

Modification (la deuxième version):  
```cpp
MPI_Request Request;
MPI_Status Status;
myvar = 0;
for ( i = 1; i < numtasks , ++i ) {
task = i;
MPI_ISend (& myvar ,... ,... , task ,...);
MPI_Wait(&Request,&Status);
myvar = myvar + 2;
// Do some works
...
MPI_Wait (...);
}
```

##Exercice3: Circulation d’un jeton dans un anneau

**Q1:**   
**Avec la commande:**  
```cpp
make SkeletonMPIProgram
```   
**Je constate que:**  
mpiexec -oversubscribe -np 16 ./SkeletonMPIProgram.exe  
Processeur n°0 sur 16 et jeton = 27  
Processeur n°1 sur 16 et jeton = 28  
Processeur n°2 sur 16 et jeton = 29  
Processeur n°3 sur 16 et jeton = 30  
Processeur n°4 sur 16 et jeton = 31  
Processeur n°5 sur 16 et jeton = 32  
Processeur n°6 sur 16 et jeton = 33  
Processeur n°7 sur 16 et jeton = 34  
Processeur n°8 sur 16 et jeton = 35  
Processeur n°9 sur 16 et jeton = 36  
Processeur n°10 sur 16 et jeton = 37  
Processeur n°11 sur 16 et jeton = 38  
Processeur n°12 sur 16 et jeton = 39  
Processeur n°13 sur 16 et jeton = 40  
Processeur n°14 sur 16 et jeton = 41  
Processeur n°15 sur 16 et jeton = 42  
Processeur n°0 sur 16 et jeton = 42  

**Parce que je utilise envoi bloquant, le processus suivant ne s'affichera qu'une fois que le processus précédent aura envoyé et reçu le jeton avec succès.**

**Q2:**  
**Avec la commande:**  
```cpp
make Circulation_jeton
```  
**Je constate que:**  
mpiexec -oversubscribe -np 16 ./Circulation_jeton.exe  
Processeur n°1 sur 16 et recu = 6  
Processeur n°2 sur 16 et recu = 6  
Processeur n°5 sur 16 et recu = 6  
Processeur n°6 sur 16 et recu = 6  
Processeur n°3 sur 16 et recu = 6  
Processeur n°13 sur 16 et recu = 8  
Processeur n°9 sur 16 et recu = 6  
Processeur n°10 sur 16 et recu = 6  
Processeur n°4 sur 16 et recu = 6  
Processeur n°11 sur 16 et recu = 6  
Processeur n°12 sur 16 et recu = 6  
Processeur n°0 sur 16 et recu = 8  
Processeur n°8 sur 16 et recu = 6  
Processeur n°7 sur 16 et recu = 6  
Processeur n°14 sur 16 et recu = 8  
Processeur n°15 sur 16 et recu = 8  


##Exercice4: Calcul de pi par lancer de fléchettes  
**L'information de le premiere version est:**  
Le premiere version  
nbSamples = 100000, nbp = 5  
The mean pi = 3.14616  
Le temps de l'execution de premiere version = 6.486ms  


**L'information de le deuxieme version est:**  
Le deuxieme version  
nbSamples = 100000, nbp = 5  
The mean pi = 3.13964  
Le temps de l'execution de premiere version = 0.446ms  

**Généralement, la méthode non bloquante est plus rapide.**  


##Exercice5: Diffusion d’un entier dans un réseau hypercube  
**Avec la commande:**    
```cpp
make Diffusion_hypercube
```  
**Je constate que:**  
mpiexec -oversubscribe -np 16 ./Diffusion_hypercube.exe  
Processeur n°1 recieves 29 for jeton from processeur n°0 dans la étape 1.  
Processeur n°9 recieves 29 for jeton from processeur n°1 dans la étape 4.  
Processeur n°5 recieves 29 for jeton from processeur n°1 dans la étape 3.  
Processeur n°2 recieves 29 for jeton from processeur n°0 dans la étape 2.  
Processeur n°3 recieves 29 for jeton from processeur n°1 dans la étape 2.  
Processeur n°8 recieves 29 for jeton from processeur n°0 dans la étape 4.  
Processeur n°11 recieves 29 for jeton from processeur n°3 dans la étape 4.  
Processeur n°10 recieves 29 for jeton from processeur n°2 dans la étape 4.  
Processeur n°4 recieves 29 for jeton from processeur n°0 dans la étape 3.  
Processeur n°6 recieves 29 for jeton from processeur n°2 dans la étape 3.  
Processeur n°12 recieves 29 for jeton from processeur n°4 dans la étape 4.  
Processeur n°13 recieves 29 for jeton from processeur n°5 dans la étape 4.  
Processeur n°14 recieves 29 for jeton from processeur n°6 dans la étape 4.  
Processeur n°7 recieves 29 for jeton from processeur n°3 dans la étape 3.  
Processeur n°15 recieves 29 for jeton from processeur n°7 dans la étape 4.  

**Et dans "Exercice 5.txt", on a:**    
Le temps d'execution est 20.9792ms.
