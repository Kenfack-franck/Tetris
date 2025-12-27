#pragma once

// Paramètres de la fenêtre
const int SCREEN_WIDTH =1000;
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

// Liste des couleurs associées aux IDs (0=Vide, 1=Cyan, 2=Jaune, etc.)
static const Color COLORS[] = {
    {0, 0, 0},       // 0: Vide (Noir/Transparent)
    {0, 255, 255},   // 1: I (Cyan)
    {255, 255, 0},   // 2: O (Jaune)
    {128, 0, 128},   // 3: T (Violet)
    {0, 255, 0},     // 4: S (Vert)
    {255, 0, 0},     // 5: Z (Rouge)
    {0, 0, 255},     // 6: J (Bleu)
    {255, 127, 0},    // 7: L (Orange)
    {128, 128, 128}  // 8: GARBAGE (Gris) <--- AJOUTÉ
};