#pragma once
#include "../Tetromino.hpp"

// I-Tetromino (Cyan) - Pièce droite
class IPiece : public Tetromino {
public:
    IPiece() : Tetromino(1) {}
    virtual ~IPiece() = default;
};

// O-Tetromino (Jaune) - Carré
class OPiece : public Tetromino {
public:
    OPiece() : Tetromino(2) {}
    virtual ~OPiece() = default;
};

// T-Tetromino (Magenta) - T
class TPiece : public Tetromino {
public:
    TPiece() : Tetromino(3) {}
    virtual ~TPiece() = default;
};

// S-Tetromino (Vert) - S
class SPiece : public Tetromino {
public:
    SPiece() : Tetromino(4) {}
    virtual ~SPiece() = default;
};

// Z-Tetromino (Rouge) - Z
class ZPiece : public Tetromino {
public:
    ZPiece() : Tetromino(5) {}
    virtual ~ZPiece() = default;
};

// L-Tetromino (Orange) - L
class LPiece : public Tetromino {
public:
    LPiece() : Tetromino(6) {}
    virtual ~LPiece() = default;
};

// J-Tetromino (Bleu) - J
class JPiece : public Tetromino {
public:
    JPiece() : Tetromino(7) {}
    virtual ~JPiece() = default;
};
