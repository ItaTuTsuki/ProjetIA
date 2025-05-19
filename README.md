# ProjetIA - Puissance N

Ce projet a pour but d'implémenter une IA sur un jeu.
Nous avons choisi le jeu du Puissance 4 et ensuite l'avons généralisé en Puissance N.

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

Sur Linux uniquement :
```bash
sudo apt update
sudo apt install pkg-config
sudo apt install build-essential libgtk-3-dev
```

## Compiler le projet 
  
```bash
make
```  

![alt text](img/image-1.png)

Cela génère un exécutable nommé puissance_n.


## Lancer le programme
  
```bash
./puissance_n
``` 

## Utilisation du programme

### Choix de l'interface
Vous pouvez choisir si vous voulez jouer dans le terminal ou dans une interface graphique.

![alt text](img/image-3.png)

### Choix du mode de jeu (Joueur VS Joueur ou Joueur VS IA)
Un choix vous est proposé entre jouer contre un vrai joueur (sur le même appareil) ou jouer contre une IA.  

![alt text](img/image-6.png)

### Paramétrer l'IA
Vous pouvez choisir à quelle profondeur l'IA va tester les coups entre 2 et 6 coups.  
2, car sinon l'IA n'est pas assez performante, et pas au-delà de 6, car avec des plateaux trop grands le temps de réflexion devient trop long pour être jouable.

![alt text](img/image-7.png)

### Choix des dimensions du plateau de jeu
Vous pouvez choisir vos paramètres personnalisés pour créer votre plateau personnel.  
Avec dimension minimale de 4x4.  
Les dimensions d'un Puissance 4 classique sont :  
lignes = 6 et colonnes = 7

![alt text](img/image-4.png)

### Choix du nombre de jetons à aligner
Vous pouvez maintenant choisir le nombre de jetons à aligner pour gagner la partie.  
Le minimum est 3 et le maximum est la valeur la plus petite entre le nombre de colonnes et de lignes.

![alt text](img/image-5.png)

### Phase de jeu dans Terminal
Vous avez juste à entrer la colonne dans laquelle vous voulez mettre votre jeton.

![alt text](img/image-9.png)
![alt text](img/image-10.png)

L'IA joue ensuite, et c'est à votre tour de jouer.

![alt text](img/image-11.png)

Le jeu se déroule jusqu’à ce qu’un joueur gagne ou que le plateau soit rempli.

![alt text](img/image-12.png)

### Phase de jeu dans GUI

L'interface graphique se compose de deux blocs :
- Les informations du plateau et le score du nombre de parties gagnées par chaque joueur
- Le plateau de jeu 

![alt text](img/image-13.png)

Passer la souris sur une colonne fait afficher en haut de cette colonne un jeton, pour bien visualiser le coup qui va être joué.

![alt text](img/image-14.png)


Pour jouer un coup, il suffit de faire un clic gauche avec la souris sur la colonne désirée.

![alt text](img/image-15.png)

De la même manière que dans le jeu dans le terminal, le jeu se termine :
-   soit quand l’un des joueurs a aligné le nombre de jetons requis
-   soit quand le plateau est plein et que plus aucun jeton ne peut y être joué

![alt text](img/image-16.png)

Ici l'IA a pu aligner horizontalement 4 jetons alors elle a gagné, et un message s'affiche pour indiquer le vainqueur et proposer de rejouer.

![alt text](img/image-17.png)

Le score se met à jour après chaque partie remportée par l'un des joueurs.

![alt text](img/image-18.png)

S'il y a match nul alors La boîte de dialogue le signale et demande si l’on souhaite rejouer, le score ne se met pas à jour dans ce cas-là.

![alt text](img/image-19.png)

Après avoir terminé de jouer, un récapitulatif des scores est affiché dans le terminal.

![alt text](img/image-20.png)








## Nettoyer les fichiers objets et exécutables générés 
  
```bash
make clean
``` 

## Auteurs
  
Emilien Derrien  
Clément Oudelet  
Hugo Exertier  
  
