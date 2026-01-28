#include "../include/NetworkManager.hpp"
#include <iostream>

NetworkManager::NetworkManager() 
    : mainSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET), 
      role(NetworkRole::NONE), isConnected(false), isNonBlocking(false), initialized(false) {
    init();
}

// Implémentation de la méthode virtuelle init() de Manager
bool NetworkManager::init() {
    if (initialized) return true;  // Déjà initialisé
    
    // 1. Initialiser Winsock (Version 2.2)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed: " << result << std::endl;
        initialized = false;
        return false;
    }
    
    initialized = true;
    return true;
}

// Implémentation de la méthode virtuelle cleanup() de Manager
void NetworkManager::cleanup() {
    if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
    if (mainSocket != INVALID_SOCKET) closesocket(mainSocket);
    WSACleanup();
    isConnected = false;
    role = NetworkRole::NONE;
    initialized = false;
}

// Implémentation de la méthode virtuelle isInitialized() de Manager
bool NetworkManager::isInitialized() const {
    return initialized;
}

NetworkManager::~NetworkManager() {
    cleanup();
}

void NetworkManager::setNonBlocking(SOCKET s) {
    u_long mode = 1; // 1 = Non-Blocking
    ioctlsocket(s, FIONBIO, &mode);
}

// Récupère l'IP locale (un peu technique, on demande au DNS local)
std::string NetworkManager::getLocalIP() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) return "Erreur";

    struct addrinfo hints = {0}, *res = nullptr;
    hints.ai_family = AF_INET; // IPv4 seulement
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(hostname, NULL, &hints, &res) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
        strcpy_s(ipStr, INET_ADDRSTRLEN, inet_ntoa(ipv4->sin_addr));
        freeaddrinfo(res);
        return std::string(ipStr);
    }
    return "Non trouvee";
}

// --- SERVEUR ---
bool NetworkManager::startServer(int port) {
    role = NetworkRole::SERVER;
    
    // 1. Créer le socket
    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET) return false;

    // 2. Préparer l'adresse (Ecouter sur toutes les interfaces locales)
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    service.sin_port = htons(port);

    // 3. Lier le socket au port (Bind)
    if (bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Bind failed." << std::endl;
        closesocket(mainSocket);
        return false;
    }

    // 4. Mettre en écoute (Listen)
    if (listen(mainSocket, 1) == SOCKET_ERROR) {
        std::cout << "Listen failed." << std::endl;
        return false;
    }

    // 5. Mettre le socket principal en NON-BLOQUANT
    // Pour ne pas figer le jeu pendant qu'on attend une connexion
    setNonBlocking(mainSocket);

    std::cout << "Serveur demarre sur le port " << port << std::endl;
    return true;
}

bool NetworkManager::checkForNewConnection() {
    if (isConnected) return true; // Déjà connecté

    // Essayer d'accepter une connexion entrante
    clientSocket = accept(mainSocket, NULL, NULL);
    
    if (clientSocket != INVALID_SOCKET) {
        std::cout << "Un client s'est connecte !" << std::endl;
        isConnected = true;
        setNonBlocking(clientSocket); // Le socket de discussion est aussi non-bloquant
        return true;
    }
    
    // Si erreur est WSAEWOULDBLOCK, ça veut juste dire "personne n'est là", ce n'est pas une vraie erreur
    return false;
}

// --- CLIENT ---
bool NetworkManager::connectToServer(std::string ip, int port) {
    role = NetworkRole::CLIENT;

    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET) return false;

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip.c_str());
    clientService.sin_port = htons(port);

    // Tenter la connexion (Bloquant pour l'instant, c'est plus simple pour le client)
    // On pourrait le faire en thread, mais pour l'instant le jeu va figer 1-2 sec max si IP invalide
    if (connect(mainSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Connexion echouee." << std::endl;
        closesocket(mainSocket);
        return false;
    }

    std::cout << "Connecte au serveur !" << std::endl;
    clientSocket = mainSocket; // En mode client, mainSocket sert à parler
    isConnected = true;
    setNonBlocking(clientSocket);
    return true;
}

// --- COMMUNICATION ---
bool NetworkManager::sendData(const std::string& data) {
    if (!isConnected) return false;
    
    // On envoie la taille d'abord ? Non, restons simple : Protocole Texte terminé par '\n'
    std::string packet = data + "\n"; 
    
    int result = send(clientSocket, packet.c_str(), (int)packet.size(), 0);
    if (result == SOCKET_ERROR) {
        std::cout << "Send failed." << std::endl;
        isConnected = false; // Déconnexion détectée
        return false;
    }
    return true;
}

std::string NetworkManager::receiveData() {
    if (!isConnected) return "";

    char recvbuf[512]; // Buffer de réception
    int recvbuflen = 512;

    int result = recv(clientSocket, recvbuf, recvbuflen - 1, 0); // -1 pour garder place pour \0
    
    if (result > 0) {
        recvbuf[result] = '\0'; // Terminer la chaine
        return std::string(recvbuf);
    } 
    else if (result == 0) {
        // Connexion fermée par l'autre
        std::cout << "Connexion fermee par le pair." << std::endl;
        isConnected = false;
    }
    else {
        // Erreur ou "Pas de données dispo" (WSAEWOULDBLOCK)
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            // Vraie erreur
            isConnected = false;
        }
    }
    return "";
}



