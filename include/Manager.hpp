#pragma once

// Classe abstraite de base pour tous les managers
// Démontre l'héritage et le polymorphisme (Séance 2 & 8)
class Manager {
public:
    virtual ~Manager() = default;

    // Initialisation du manager
    virtual bool init() = 0;

    // Nettoyage et libération des ressources
    virtual void cleanup() = 0;

    // Vérifier si le manager est initialisé
    virtual bool isInitialized() const = 0;
};
