

# TP2 de WANG Yu

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`





## lscpu

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
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss syscall nx pdpe1gb rdtscp lm constant_tsc arch_perfmon nopl xtopology tsc_reliable nonstop_tsc cpuid pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch cpuid_fault invpcid_single pti ssbd ibrs ibpb stibp fsgsbase tsc_adjust bmi1 avx2 smep bmi2 invpcid rdseed adx smap xsaveopt arat md_clear flush_l1d arch_capabilities
```

*Des infos utiles s'y trouvent : nb core, taille de cache*



## Produit scalaire 

*Expliquer les paramètres, les fichiers, l'optimisation de compil, NbSamples, ...*

OMP_NUM    | samples=1024 
-----------|-------------------
séquentiel | 0.868233
1          |   0.281294
2          |   0.171663 
3          |   0.169588
4          |   0.159374
5          |   0.257736
6          |   0.174712
7          |   0.156034
8          |   0.153738


*Discuter sur ce qu'on observe, la logique qui s'y cache.*


## Produit matrice-matrice



### Permutation des boucles

*Expliquer comment est compilé le code (ligne de make ou de gcc) : on aura besoin de savoir l'optim, les paramètres, etc. Par exemple :*

`make TestProduct.exe && ./TestProduct.exe 1024`


  ordre           | time    | MFlops  | MFlops(n=2048) 
------------------|---------|---------|----------------
i,j,k (origine)   | 10.5237 | 204.063 | 164.133          
j,i,k             | 10.8489 | 197.945 | 158.581
i,k,j             | 28.0939 | 76.4394 | 73.3661   
k,i,j             | 24.378 | 88.0911 | 71.4221
j,k,i             | 0.917265 | 2341.18 | 2452.97
k,j,i             | 1.15568 | 1858.2 | 1739.47 


*Discussion des résultats*



### OMP sur la meilleure boucle 

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProduct.exe 1024`

  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 2215.41 | 2160.2 | 2107.57 | 2099.42
2                 | 3585.08 | 3914.91 | 3219.52 | 3834.99
3                 | 3675.65 | 3059.66 | 3250.56 | 3749.34
4                 | 3740.57 | 3782.46 | 3417.77 | 3878.3
5                 | 3795.95 | 3832.11 | 3271.63 | 3830.39
6                 | 3729.3 | 3660.87 | 3142.98 | 3525.03
7                 | 3724.77 | 3841.95 | 3108.07 | 3742.91
8                 | 3704.09 | 3743.39 | 3065.91 | 3734.1




### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    |  |  |  | 
32                | 2398.43 | 2390.3 | 2244.74 | 2419.23
64                | 2279.64 | 2394.08 | 2257.98 | 2401.96
128               | 2535.21 | 2555 | 2390.34 | 2564.1
256               | 2687.75 | 2664.24 | 2485.02 | 2709.98
512               | 2316.39 | 2320.05 | 2349.03 | 2328.87
1024              | 2238.74 | 2107.46 | 2428.85 | 1987.84




### Bloc + OMP

szBlock = 256

OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 1936.01 | 2043.81 | 2107.57 | 2200.83
2                 | 4197.84 | 4104.5 | 3208.33 | 4204.27
3                 | 3944.83 | 3963.88 | 2934.13 | 3886.65
4                 | 3785.52 | 3746.81 | 2906.11 | 3454.65
5                 | 3768.06 | 3900.78 | 2752.71 | 4001.9
6                 | 3637.92 | 3594.11 | 2692.72 | 3893.58
7                 | 3577.73 | 3741.67 | 2617.42 | 3805.54
8                 | 3466.07 | 3480.43 | 2560.29 | 3823.03







# Tips 

```
	env 
	OMP_NUM_THREADS=4 ./dot_product.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```
