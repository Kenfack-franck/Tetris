# Tetris POO - Projet

## 📋 Table des matières

- [À propos du projet](#-à-propos-du-projet)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Compilation](#-compilation)
- [Exécution](#-exécution)
- [Gameplay](#-gameplay)
- [Architecture du projet](#-architecture-du-projet)
- [Technologies utilisées](#-technologies-utilisées)
- [Notes d'implémentation](#-notes-dimplémentation)

## 🎮 À propos du projet

Tetris POO est une implémentation du jeu Tetris classique en C++ utilisant une approche **orientée objet**. Le projet supporte actuellement le mode de jeu solo et dispose d'une architecture modulaire extensible.

## 🛠️ Prérequis

### Système d'exploitation
- Windows 10/11 ou Linux

### Bibliothèques requises
- SDL2 2.30.0 ou supérieure
- SDL2_ttf 2.22.0 ou supérieure

### Compilateur
- MinGW-w64 avec GCC 14.2.0 (inclus dans le projet)

## 📦 Installation

1. **Télécharger ou cloner le projet**
   ```
   git clone <repository>
   cd tetris
   ```

2. **Les dépendances sont préemballées**
   - SDL2 se trouve dans : `SDL2-devel-2.30.0-mingw/`
   - SDL2_ttf se trouve dans : `SDL2_ttf-devel-2.22.0-mingw/`
   - Les DLL compilées se trouvent dans : `lib_external/`

3. **Copier les DLL de runtime MinGW** (pour éviter les erreurs de DLL manquantes)
   ```powershell
   Copy-Item ".\lib\winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2\mingw64\bin\libgcc_s_seh-1.dll" .
   Copy-Item ".\lib\winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2\mingw64\bin\libstdc++-6.dll" .
   Copy-Item ".\lib\winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2\mingw64\bin\libwinpthread-1.dll" .
   ```

## 🚀 Compilation

Lancez la compilation avec la commande suivante (depuis le dossier `tetris/`) :

```powershell
& ".\lib\winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2\mingw64\bin\g++.exe" src/*.cpp -I lib_external/include -L lib_external/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -o main.exe
```

### Résolution des problèmes courants

| Erreur | Solution |
|--------|----------|
| `undefined reference to 'TTF_Init'` | Ajouter `-lSDL2_ttf` à la ligne de compilation |
| `libgcc_s_seh-1.dll is missing` | Copier les DLL de runtime (voir section Installation) |
| Fenêtre ne s'affiche pas | Vérifier que SDL2 est correctement lié |

## ▶️ Exécution

Lancez le jeu depuis le dossier `tetris/` :

```powershell
.\main.exe
```

## 🎮 Gameplay

### Règles
- Compléter des lignes horizontales pour les éliminer
- Empiler les pièces sans remplir l'écran
- Le jeu se termine lorsque les pièces atteignent le sommet
- Gagnez des points en complétant des lignes

### Contrôles du clavier
| Touche | Action |
|--------|--------|
| ⬅️ Gauche | Déplacer la pièce à gauche |
| ➡️ Droite | Déplacer la pièce à droite |
| ⬇️ Bas | Accélérer la chute |
| 🔄 Haut / Z | Tourner la pièce |
| ESC / Q | Quitter le jeu |
| ESPACE | Pause (si implémentée) |

### Pièces (Tétrominos)
- **I** (Cyan) : Ligne droite
- **O** (Jaune) : Carré
- **T** (Violet) : Forme en T
- **S** (Vert) : Zigzag droit
- **Z** (Rouge) : Zigzag gauche
- **J** (Bleu) : L inversé
- **L** (Orange) : L droit

## 📁 Architecture du projet

```
tetris/
├── src/                      # Code source
│   ├── main.cpp             # Point d'entrée du programme
│   ├── Game.cpp             # Logique principale du jeu
│   ├── Board.cpp            # Gestion du plateau
│   ├── Tetromino.cpp        # Représentation des pièces
│   └── TetrisInstance.cpp   # Instance de jeu (joueur)
│
├── include/                  # Fichiers en-têtes
│   ├── Game.hpp             # Classe Game
│   ├── Board.hpp            # Classe Board
│   ├── Tetromino.hpp        # Classe Tetromino
│   ├── TetrisInstance.hpp   # Classe TetrisInstance
│   └── Defs.hpp             # Constantes et définitions
│
├── lib_external/            # Bibliothèques compilées
│   ├── include/             # En-têtes SDL2 et SDL2_ttf
│   └── lib/                 # Fichiers .a et .la
│
└── README.md                # Ce fichier
```

### Modules principaux

- **Game** : Classe principale orchestrant la logique du jeu
- **Board** : Gère le plateau de jeu (dimensions, état des cellules)
- **Tetromino** : Représente les pièces et leurs rotations
- **TetrisInstance** : Encapsule un joueur avec son propre plateau et pièce active

## 🔧 Technologies utilisées

- **Langage** : C++ (standard C++11)
- **Approche** : Programmation Orientée Objet (POO)
- **Rendering** : SDL2 (Simple DirectMedia Layer)
- **Typographie** : SDL2_ttf (pour l'affichage du texte)
- **Compilateur** : MinGW-w64 GCC 14.2.0
- **Plateforme cible** : Windows 10/11

## 📝 Notes d'implémentation

### Points clés du design
- Architecture modulaire avec séparation des responsabilités
- Utilisation de classes pour encapsuler les données et la logique
- Gestion centralisée des événements SDL2
- Support potentiel du multijoueur (deux joueurs définis dans Game)

### Défis rencontrés
- Configuration des chemins d'inclusion SDL2 et SDL2_ttf
- Gestion des dépendances runtime MinGW sur Windows
- Rotation des tétrominos (implémentation des 4 orientations)
- Synchronisation des événements input et update/render

### Améliorations futures
- Interface utilisateur améliorée (menu, paramètres)
- Mode multijoueur fonctionnel
- Système de score persistant
- Niveaux de difficulté
- Effets sonores
- Animations fluides
