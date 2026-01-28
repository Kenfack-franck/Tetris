# TetrENSTA - Projet C++ Orienté Objet

## 📋 Table des matières

- [À propos du projet](#-à-propos-du-projet)
- [Aperçu Visuel](#-aperçu-visuel)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Compilation](#-compilation)
- [Exécution](#-exécution)
- [Gameplay](#-gameplay)
- [Architecture du projet](#-architecture-du-projet)
- [Concepts C++ Implémentés](#-concepts-c-implémentés)
- [Technologies utilisées](#-technologies-utilisées)
- [Notes d'implémentation](#-notes-dimplémentation)

## 🎮 À propos du projet

**TetrENSTA** est une implémentation complète du jeu Tetris en C++ utilisant une approche **orientée objet**. Le projet démontre plusieurs concepts avancés de C++ (Séance 1-13 du cours) et supporte le mode solo et multijoueur en réseau.

### Caractéristiques Principales
- ✅ Gameplay Tetris complet avec deux joueurs simultanés
- ✅ Système de score et niveaux de difficulté
- ✅ Support multijoueur en réseau (TCP/IP)
- ✅ Interface graphique attrayante avec avatars des joueurs
- ✅ Effets sonores et musique de fond
- ✅ Architecture modulaire et extensible

## 📸 Aperçu Visuel

### Écran Principal du Menu
![Menu TetrENSTA](assets/menu_bg.png)

Le menu d'accueil avec les avatars des joueurs et les options de jeu (solo ou réseau).

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
& ".\lib\winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r2\mingw64\bin\g++.exe" src/*.cpp -I lib_external/include -L lib_external/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lws2_32 -o main.exe
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
| **Q, Z, D, S** ou **Flèches** | Déplacer la pièce |
| **Espace** | Chute rapide (Hard Drop) |
| **C** ou **M** | Hold (Réserver une pièce) |
| **P** | Pause |
| **ESC** | Quitter le jeu |
| **L** | Mode Local (2 joueurs) |
| **H** | Mode Hôte (Multijoueur Réseau) |
| **J** | Rejoindre (Multijoueur Réseau) |

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
├── src/                      # Code source C++
│   ├── main.cpp             # Point d'entrée du programme
│   ├── Game.cpp             # Logique principale du jeu
│   ├── Board.cpp            # Gestion du plateau de jeu
│   ├── Tetromino.cpp        # Représentation des pièces (classe de base)
│   ├── TetrisInstance.cpp   # Instance de jeu pour chaque joueur
│   ├── AudioManager.cpp     # Gestion audio et musique
│   ├── NetworkManager.cpp   # Gestion du réseau TCP/IP
│   └── pieces/              # Hiérarchie des Tétrominos
│       └── Pieces.cpp       # Classes dérivées (IPiece, OPiece, etc.)
│
├── include/                  # Fichiers en-têtes (.hpp)
│   ├── Game.hpp             # Classe Game (orchestratrice)
│   ├── Board.hpp            # Classe Board (plateau)
│   ├── Tetromino.hpp        # Classe de base Tetromino
│   ├── TetrisInstance.hpp   # Classe TetrisInstance (joueur)
│   ├── Manager.hpp          # Classe abstraite Manager
│   ├── AudioManager.hpp     # Classe AudioManager (héritage de Manager)
│   ├── NetworkManager.hpp   # Classe NetworkManager (héritage de Manager)
│   ├── pieces/
│   │   └── Pieces.hpp       # Classes dérivées de Tetromino
│   └── Defs.hpp             # Constantes et définitions
│
├── assets/                   # Ressources du jeu
│   ├── menu_bg.png          # Image de fond du menu
│   ├── avatar1.bmp          # Avatar joueur 1
│   ├── avatar2.bmp          # Avatar joueur 2
│   ├── font.ttf             # Police de caractères
│   └── music*.wav/mp3       # Effets sonores et musique
│
├── lib_external/            # Bibliothèques compilées
│   ├── include/             # En-têtes SDL2, SDL2_ttf, SDL2_mixer
│   └── lib/                 # Fichiers .a et .la compilés
│
└── README_new.md            # Ce fichier
```

### Modules principaux

| Classe | Responsabilité |
|--------|-----------------|
| **Game** | Orchestration principale, gestion des états (Menu, Play, Pause, GameOver) |
| **TetrisInstance** | Encapsule un joueur avec son plateau, pièces et logique locale |
| **Board** | Gestion du plateau (dimensions, collisions, effacement de lignes) |
| **Tetromino** (classe de base) | Représente une pièce et ses transformations (rotation, mouvement) |
| **IPiece, OPiece, TPiece, etc.** | Sous-classes spécialisées pour chaque type de Tétromino |
| **Manager** (abstraite) | Interface commune pour les gestionnaires de ressources |
| **AudioManager** | Gestion de la musique et des effets sonores (héritage de Manager) |
| **NetworkManager** | Gestion de la communication TCP/IP pour le multijoueur (héritage de Manager) |

## 🎓 Concepts C++ Implémentés

### Séance 1: Introduction aux Objets ✅
- Définition de classes avec encapsulation (private/public)
- Constructeurs et destructeurs
- Membres et méthodes d'instance
- Exemples: `Game`, `Board`, `Tetromino`, `TetrisInstance`

### Séance 2 & 8: Héritage & Polymorphisme ✅
- **Hiérarchie Manager**: 
  - Classe abstraite `Manager` avec méthodes virtuelles
  - `AudioManager` et `NetworkManager` héritent de `Manager`
- **Hiérarchie Tetromino**:
  - Classe de base `Tetromino` avec destructeur virtuel
  - 7 sous-classes spécialisées: `IPiece`, `OPiece`, `TPiece`, `SPiece`, `ZPiece`, `LPiece`, `JPiece`
  - Polymorphisme virtuel pour les méthodes `draw()` et `update()`

### Séance 4: STL - Conteneurs & Itérateurs ✅
- `std::vector<std::vector<int>>` pour la grille du plateau
- `std::vector` pour les formes des pièces
- `std::string` pour les messages et communications réseau

### Séance 11: Pointeurs Intelligents ✅
- `std::unique_ptr<TetrisInstance>` pour les joueurs (player1, player2)
- `std::make_unique<T>()` pour l'allocation gérée
- **Avantages**: Destruction automatique, exception-safe, zéro memory leak

### Partiellement Implémenté ⚠️
- **Séance 5**: Opérateurs et flux (`std::cout`, `std::string`, assignation)
- **Séance 3**: Conteneurs STL (utilisation de `std::vector`)

### Non Implémenté ❌
- Séance 6: C++20 (Code compilé en C++17)
- Séance 7: Gestion d'exceptions (try/catch)
- Séance 9: Parallélisme et programmation asynchrone
- Séance 12: Évaluation compile-time (constexpr)
- Séance 13: Modules C++20

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
