# ProjetIA - Puissance N

Ce projet a pour but d'implémenter une IA sur un jeu.
Nous avons choisi le jeux du puissance 4 et ensuite l'avons généralisé en puissance N.

## Structure du projet
<pre markdown>
puissance_n/  
├── include/                          # Fichiers d'en-tête (.h)  
│   ├── ai.h                                   # Prototypes de l'IA (Minimax)  
│   ├── game.h                                 # Fonctions liées au plateau de jeu  
│   └── graphical_interface.h                  # Interface graphique (GTK)  
│  
├── src/                              # Code source (.c)  
│   ├── ai.c                                   # Implémentation de l'IA  
│   ├── game.c                                 # Logique du jeu (init, coup, victoire)  
│   ├── graphical_interface.c                  # Affichage graphique, GTK, animation  
│   └── main.c                                 # Point d’entrée (menu terminal ou graphique)  
│  
├── puissance_n                       # Exécutable compilé  
├── Makefile                          # Script de compilation  
</pre>
  
## Prérequis
  
- **gcc**  
- **pkg-config**  
- **GTK 3**  
  
### Installer avec :

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev
```

## Compiler le projet 
  
```bash
make
```  

Cela génère un exécutable nommé puissance_n.


## Lancer le programme
  
```bash
./puissance_n
``` 


## Nettoyer les fichiers objets et executables générés 
  
```bash
make clean
``` 

## Auteurs
  
Emilien Derrien  
Clément Oudelet  
Hugo Exertier  
  
