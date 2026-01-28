#pragma once

// Paramètres de la fenêtre
const int SCREEN_WIDTH =1200;
const int SCREEN_HEIGHT = 750;

// Paramètres du jeu Tetris
const int BLOCK_SIZE = 30; // Taille d'un carré en pixels
const int BOARD_WIDTH = 10; // Nombre de colonnes
const int BOARD_HEIGHT = 20; // Nombre de lignes

// Calcul pour centrer le plateau à l'écran
const int BOARD_START_X = (SCREEN_WIDTH - (BOARD_WIDTH * BLOCK_SIZE)) / 2;
const int BOARD_START_Y = (SCREEN_HEIGHT - (BOARD_HEIGHT * BLOCK_SIZE)) / 2;

// Structure simple pour les couleurs
struct Color {
    int r, g, b;
};

// Structure pour les couleurs 3D avec bevel/shadow
struct ColorWithShading {
    Color base;      // Couleur principale
    Color light;     // Pour highlight
    Color dark;      // Pour shadow
};

// Liste des couleurs de base améliorées et plus vibrantes
static const Color COLORS[] = {
    {0, 0, 0},       // 0: Vide
    {0, 240, 255},   // 1: I (Cyan vibrant)
    {255, 235, 50},  // 2: O (Jaune vibrant)
    {180, 50, 255},  // 3: T (Violet vibrant)
    {50, 240, 100},  // 4: S (Vert vibrant)
    {255, 50, 100},  // 5: Z (Rouge vibrant)
    {50, 150, 255},  // 6: J (Bleu vibrant)
    {255, 150, 50},  // 7: L (Orange vibrant)
    {140, 140, 140}  // 8: GARBAGE (Gris)
};

// Couleurs 3D pour les blocs verrouillés avec effets lumineux
static const ColorWithShading SHADED_COLORS[] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},                    // 0: Vide
    {{0, 240, 255}, {100, 255, 255}, {0, 150, 200}},      // 1: Cyan
    {{255, 235, 50}, {255, 255, 150}, {200, 180, 0}},     // 2: Jaune
    {{180, 50, 255}, {220, 120, 255}, {120, 20, 180}},    // 3: Violet
    {{50, 240, 100}, {150, 255, 180}, {20, 180, 70}},     // 4: Vert
    {{255, 50, 100}, {255, 150, 180}, {200, 0, 50}},      // 5: Rouge
    {{50, 150, 255}, {150, 200, 255}, {20, 80, 200}},     // 6: Bleu
    {{255, 150, 50}, {255, 220, 150}, {200, 100, 0}},     // 7: Orange
    {{140, 140, 140}, {190, 190, 190}, {80, 80, 90}}      // 8: Gris
};