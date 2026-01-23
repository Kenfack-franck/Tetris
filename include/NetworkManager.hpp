#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Dit au compilateur de lier la librairie
#else
    // Si un jour tu veux porter sur Linux, il faudra d'autres includes ici
#endif

#include <string>
#include <vector>

// Définition des rôles
enum class NetworkRole {
    NONE,
    SERVER,
    CLIENT
};

class NetworkManager {
private:
    WSADATA wsaData;
    SOCKET mainSocket;   // Le socket d'écoute (pour le serveur) ou de connexion (pour le client)
    SOCKET clientSocket; // Le socket qui relie au joueur adverse
    
    NetworkRole role;
    bool isConnected;
    bool isNonBlocking;  // Pour savoir si on a activé le mode non-bloquant

    // Helper pour mettre un socket en mode non-bloquant
    void setNonBlocking(SOCKET s);

public:
    NetworkManager();
    ~NetworkManager();

    // Initialisation
    bool init(); 
    void cleanup();

    // Gestion de connexion
    std::string getLocalIP(); // Récupère l'IP de la machine (Code Partie)
    
    // Mode Serveur (Hôte)
    bool startServer(int port);
    bool checkForNewConnection(); // Appelé à chaque frame en attente

    // Mode Client (Rejoindre)
    bool connectToServer(std::string ip, int port);

    // Communication (Envoi/Réception)
    // Retourne true si succès
    bool sendData(const std::string& data);
    
    // Retourne "" si rien reçu, sinon la chaine reçue
    std::string receiveData();

    // Getters
    bool getConnected() const { return isConnected; }
    NetworkRole getRole() const { return role; }
};