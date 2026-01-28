# Configuration des Images - TETRIS VERSUS

Le jeu est maintenant configuré pour supporter les images PNG/JPG ! Voici comment mettre en place vos propres images.

## 🖼️ Images Supportées

### 1. **Fond du Menu** (`assets/menu_bg.png`)
- Dimensions recommandées: **1190 x 700 pixels**
- Format: PNG ou JPG
- Description: Image de fond pour le menu principal
- Sera affichée avec un overlay semi-transparent pour améliorer la lisibilité du texte

**Chemin:** `assets/menu_bg.png`

### 2. **Avatar Joueur 1** (`assets/avatar1.png`)
- Dimensions: **80 x 80 pixels** (carrée)
- Format: PNG avec transparence recommandée
- Description: Portrait/avatar du joueur 1 (couleur cyan/bleu)

**Chemin:** `assets/avatar1.png`

### 3. **Avatar Joueur 2** (`assets/avatar2.png`)
- Dimensions: **80 x 80 pixels** (carrée)
- Format: PNG avec transparence recommandée
- Description: Portrait/avatar du joueur 2 (couleur magenta/rose)

**Chemin:** `assets/avatar2.png`

---

## 📝 Instructions d'Installation

### Étape 1: Préparer vos images
- Créez ou téléchargez vos images
- Redimensionnez-les aux dimensions recommandées
- Convertissez-les au format PNG (transparent) ou JPG

### Étape 2: Placer les images
1. Naviguez jusqu'au dossier `assets/` du projet
2. Placez vos images avec les noms exacts:
   - `menu_bg.png` → fond du menu
   - `avatar1.png` → avatar J1
   - `avatar2.png` → avatar J2

### Étape 3: Relancer le jeu
```bash
./tetris_enhanced.exe
```

Le jeu chargera automatiquement vos images !

---

## 🔄 Système de Fallback

Si une image est manquante ou corrompue:
- **Menu:** Affiche un dégradé bleu/noir à la place
- **Avatars:** Affiche des carrés colorés simples (cyan/magenta)

Le jeu continuera à fonctionner normalement.

---

## 💡 Conseils Graphiques

### Pour le fond du menu:
- Utilisez des couleurs sombres (bleu marine, noir) pour la lisibilité
- Évitez les motifs trop chargés
- Ajoutez un peu de transparence (le jeu ajoute un overlay)

### Pour les avatars:
- Utilisez des images carrées sans déformation
- Fond transparent (PNG) pour meilleur résultat
- Bordures/contours clairs pour que l'avatar se détache

---

## 📦 Technologie

**À faire ultérieurement:** Installer SDL2_image pour un support officiel des images.

Commande future:
```bash
# (Une fois SDL2_image installé)
# Le jeu chargera automatiquement les PNG/JPG
```

Pour l'instant, le système est configuré mais utilise des fallbacks visuels.

---

## ❓ FAQ

**Q: Mes images ne s'affichent pas!**
- Vérifiez les noms exacts de fichier
- Assurez-vous qu'elles sont dans le dossier `assets/`
- Essayez de convertir en PNG si c'est du JPG

**Q: Quelle taille pour menu_bg.png?**
- 1190 x 700 pixels (résolution de l'écran)

**Q: Les avatars peuvent être plus gros?**
- Oui, modifiez le code (ligne `drawAvatar(150, 150, 80, ...)` → changez `80`)

**Q: Comment ajouter plus d'images?**
- Modifiez `Game.hpp` pour ajouter de nouvelles textures
- Chargez-les dans le constructeur Game::Game()
