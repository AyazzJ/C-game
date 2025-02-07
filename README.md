# Jeu de Tic-Tac-Toe Hiérarchique (SDL)

Ce projet est une implémentation d'un jeu de **Tic-Tac-Toe hiérarchique** en C utilisant **SDL2** et **SDL_ttf**.

## 📌 Fonctionnalités
- Mode **Joueur vs Joueur** et **Joueur vs Ordinateur**.
- Interface graphique avec **SDL2**.
- Gestion des sous-jeux (mini-grilles de Tic-Tac-Toe dans une grille principale).
- Détection automatique des victoires et des égalités.

## 🛠️ Installation et Exécution

### 1️⃣ Prérequis
Avant d'exécuter le jeu, assurez-vous d'avoir installé les bibliothèques suivantes :
- **SDL2**
- **SDL_ttf**

Sous **MSYS2** (si vous utilisez MinGW), installez-les avec :
```sh
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
2️⃣ Compilation du projet
Utilisez la commande suivante pour compiler votre code :

sh
Copy
Edit
gcc main.c -o jeu.exe -lSDL2 -lSDL2_ttf -lm
3️⃣ Lancer le jeu
Après la compilation, exécutez le jeu avec :

sh
Copy
Edit
./jeu.exe
🎮 Commandes
Souris : Cliquez sur une case pour jouer.
Clavier :
1 : Sélectionner Joueur vs Joueur.
2 : Sélectionner Joueur vs Ordinateur.
📝 À propos du projet
Ce projet a été développé pour un exercice de programmation en C utilisant SDL2. Il permet de comprendre la gestion des interfaces graphiques et de l'intelligence artificielle basique.