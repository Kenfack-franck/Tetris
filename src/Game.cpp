#include "../include/Game.hpp"
#include "../include/Defs.hpp"
#include <iostream> // Pour afficher les erreurs dans la console
#include "../include/Game.hpp"

Game::Game(SDL_Renderer* ren) 
    : renderer(ren), 
      isGameRunning(true),
      currentState(GameState::MENU), // On commence au MENU
      player1(nullptr),              // IMPORTANT
      player2(nullptr)               // IMPORTANT
{
    // 1. D'ABORD : Initialiser la librairie de texte
    if (TTF_Init() == -1) {
        std::cout << "ERREUR CRITIQUE : TTF_Init n'a pas pu demarrer ! " << TTF_GetError() << std::endl;
    }

    // Lancer la musique au démarrage
    audio.playMusic();

    // 2. ENSUITE : Charger la police
    // Assure-toi que le fichier s'appelle bien "font.ttf" et est dans le dossier "assets"
    font = TTF_OpenFont("assets/font.ttf", 24);

    if (!font) {
        std::cout << "ERREUR : Impossible de charger assets/font.ttf ! " << TTF_GetError() << std::endl;
        std::cout << "Essai de chargement de Arial par defaut..." << std::endl;
        // Essai de secours (Windows)
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }

    if (font) {
        std::cout << "Succes : Police chargee !" << std::endl;
    }

    // Init Réseau
    if (!net.init()) {
        std::cout << "Erreur Init Reseau !" << std::endl;
    }

    resetGame();

    // // INSTANCIATION DES DEUX JOUEURS
    // // Joueur 1 à Gauche (x=50)
    // player1 = new TetrisInstance(renderer, font, &audio, 50, 50);

    // // Joueur 2 à Droite (x=550) -> J'ai décalé un peu plus à droite pour éviter le chevauchement
    // // Largeur plateau = 300px (10 blocs * 30px). 50 + 300 + Marge = 450 c'était un peu juste.
    // player2 = new TetrisInstance(renderer, font,&audio, 600, 50); 
}

void Game::handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {

        // --- MENU PRINCIPAL ---
        if (currentState == GameState::MENU) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                currentState = GameState::MODE_SELECTION; // On va au choix du mode
            }

            // --- AJOUT ICI ---
            // ECHAP : Fermer le jeu complètement
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isGameRunning = false;
            }
        }
        

        // --- SÉLECTION DU MODE (Local ou Réseau ?) ---
        else if (currentState == GameState::MODE_SELECTION) {
            // 1. JOUER EN LOCAL (Même clavier)
            if (event.key.keysym.sym == SDLK_l) { // L pour Local
                isNetworkGame = false;
                resetGame(); // Prépare les 2 joueurs locaux
                currentState = GameState::PLAY;
            }
            // 2. JOUER EN RÉSEAU (Héberger)
            else if (event.key.keysym.sym == SDLK_h) { // H pour Host
                isNetworkGame = true;
                net.startServer(12345);
                currentState = GameState::MULTI_HOST;
            }
            // 3. JOUER EN RÉSEAU (Rejoindre)
            else if (event.key.keysym.sym == SDLK_j) { // J pour Join
                isNetworkGame = true;
                inputText = "";
                connectionStatus = "";
                SDL_StartTextInput();
                currentState = GameState::MULTI_JOIN;
            }
            // 4. RETOUR AU TITRE
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::MENU;
            }
        }

        // --- SALLE D'ATTENTE HÔTE ---
        else if (currentState == GameState::MULTI_HOST) {
            // RETOUR ARRIÈRE : On annule l'hébergement
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                net.cleanup(); // Coupe le serveur
                net.init();    // Réinitialise Winsock
                currentState = GameState::MODE_SELECTION;
            }
        }

        // --- SAISIE IP CLIENT ---
        else if (currentState == GameState::MULTI_JOIN) {

            // RETOUR ARRIÈRE : On annule la saisie
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                SDL_StopTextInput();
                currentState = GameState::MODE_SELECTION;
            }

            if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0) {
                inputText.pop_back(); // Effacer caractère
            }
            else if (event.key.keysym.sym == SDLK_RETURN) {

                // 1. Afficher qu'on essaie
                connectionStatus = "Connexion en cours...";
                
                // Petite astuce pour forcer l'affichage du message tout de suite 
                // (sinon le jeu fige pendant la connexion sans afficher le message)
                render(); 

                SDL_StopTextInput();
                // Tenter la connexion avec l'IP saisie
                if (net.connectToServer(inputText, 12345)) {
                    connectionStatus = ""; // On vide le message
                    resetGame();
                    currentState = GameState::PLAY;
                } else {
                    // 3. En cas d'échec
                    connectionStatus = "Echec : Code invalide ou Hote introuvable.";
                    SDL_StartTextInput(); // On laisse le clavier actif pour corriger
                }
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                SDL_StopTextInput();
                currentState = GameState::MODE_SELECTION;
            }
        }


        // --- ÉTAT PLAY ---
        else if (currentState == GameState::PLAY) {
            // Pause (Autorisée seulement en local pour l'instant pour simplifier)
            if (!isNetworkGame && (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE)) {
                currentState = GameState::PAUSE;
                return;
            }
            
            // Si en réseau : Echap pour ragequit/menu ? (À gérer plus tard)
            
            // --- CONTRÔLES JOUEUR 1 ---
            // Actif si : Pas de réseau (Local) OU Je suis le Serveur
            if (!isNetworkGame || net.getRole() == NetworkRole::SERVER) {
                std::string cmd = "";
                switch (event.key.keysym.sym) {
                    case SDLK_q: case SDLK_a: player1->moveLeft(); cmd="L"; break;
                    case SDLK_d:              player1->moveRight(); cmd="R"; break;
                    case SDLK_z: case SDLK_w: player1->rotate(); cmd="ROT"; break;
                    case SDLK_s:              player1->softDrop(); cmd="SOFT"; break;
                    case SDLK_SPACE:          player1->hardDrop(); cmd="DROP"; break;
                    case SDLK_c:              player1->hold(); cmd="HOLD"; break;
                }
                // Si une commande a été faite et qu'on est en réseau, on l'envoie
                if (isNetworkGame && !cmd.empty()) net.sendData(cmd);
            }

            // --- CONTRÔLES JOUEUR 2 ---
            // Actif si : Pas de réseau (Local) OU Je suis le Client
            if (!isNetworkGame || net.getRole() == NetworkRole::CLIENT) {
                std::string cmd = "";
                // En réseau client, on peut décider d'utiliser aussi ZQSD ou Flèches.
                // Pour l'instant, disons qu'on utilise les flèches pour J2.
                
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:     player2->moveLeft(); cmd="L"; break;
                    case SDLK_RIGHT:    player2->moveRight(); cmd="R"; break;
                    case SDLK_UP:       player2->rotate(); cmd="ROT"; break;
                    case SDLK_DOWN:     player2->softDrop(); cmd="SOFT"; break;
                    case SDLK_RETURN:   case SDLK_KP_ENTER: player2->hardDrop(); cmd="DROP"; break;
                    case SDLK_m:        player2->hold(); cmd="HOLD"; break;
                }
                if (isNetworkGame && !cmd.empty()) net.sendData(cmd);
            }
        }
    
        
    // --- ÉTAT PAUSE ---
        else if (currentState == GameState::PAUSE) {
            if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::PLAY; // Reprendre
            }
            // QUITTER LA PARTIE (Ragequit propre)
            if (event.key.keysym.sym == SDLK_q) {
                // Si on était en réseau, on coupe tout
                if (isNetworkGame) {
                    net.cleanup();
                    net.init();
                }
                
                audio.playMusic(); // Relance musique menu
                currentState = GameState::MODE_SELECTION; // Retour au choix
            }
        }

        // --- ÉTAT GAME OVER ---
        else if (currentState == GameState::GAME_OVER) {
            if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {

                 // Si on est en réseau, on envoie l'ordre de RESTART
                if (isNetworkGame) {
                    net.sendData("RESTART");
                }

                // On reset localement
                // ATTENTION : En réseau, il faut renvoyer le SEED aussi ? 
                // Simplifions : On relance juste resetGame().
                
                // Petite astuce : En réseau, seul l'Hôte devrait avoir le droit de relancer
                // ou alors les deux peuvent. Disons que les deux peuvent.
                resetGame();
                currentState = GameState::PLAY;
            }

            if (event.key.keysym.sym == SDLK_ESCAPE) {
                // Si on était en réseau, il faut se déconnecter proprement
                net.cleanup();
                net.init(); // On prépare pour une prochaine fois

                // Au lieu de retourner au MENU Principal (Titre), on retourne au choix Multi
                // pour pouvoir relancer ou rejoindre vite.
                currentState = GameState::MODE_SELECTION;
                
                audio.playMusic();
            }
        }
    }

    // --- GESTION SAISIE TEXTE (Pour l'IP) ---
    else if (event.type == SDL_TEXTINPUT && currentState == GameState::MULTI_JOIN) {
        inputText += event.text.text; // Ajouter le caractère tapé
    }

}

void Game::update() {

    // --- UPDATE EN ATTENTE HÔTE ---
    if (currentState == GameState::MULTI_HOST) {
        // Est-ce qu'un client arrive ?
        if (net.checkForNewConnection()) {
            std::cout << "Client connecte ! Lancement du jeu..." << std::endl;

            // 1. Synchronisation du Hasard
            // On utilise l'heure actuelle pour avoir un nombre unique
            unsigned int seed = (unsigned int)time(NULL);
            srand(seed); // Je l'applique à moi (Hôte)
            
            // J'envoie la graine au client
            std::string msg = "SEED:" + std::to_string(seed);
            net.sendData(msg);

            resetGame();
            currentState = GameState::PLAY;
            
            // Envoyer un "HELLO" pour synchroniser (optionnel)
            net.sendData("START");
        }
        return;
    }


    // --- RÉCEPTION RÉSEAU (Lecture des paquets) ---
    if (isNetworkGame && (currentState == GameState::PLAY || currentState == GameState::GAME_OVER)) {
        // On lit tout ce qui est arrivé
        std::string rawData = net.receiveData();
        
        // receiveData peut renvoyer plusieurs commandes collées (ex: "L\nR\n")
        // Il faudrait idéalement un buffer, mais pour simplifier, on traite le dernier message 
        // ou on suppose qu'on traite packet par packet.
        // NOTE: Mon NetworkManager::receiveData simplifié ne lit qu'un bloc. 
        // S'il contient des \n, il faut le découper.
        
        // Simplification : On traite 'rawData'
        if (!rawData.empty()) {
            // Qui est l'autre joueur ?
            TetrisInstance* remote = (net.getRole() == NetworkRole::SERVER) ? player2 : player1;

            // Découpage sommaire par ligne (si plusieurs commandes arrivent vite)
            size_t pos = 0;
            while ((pos = rawData.find('\n')) != std::string::npos) {
                std::string cmd = rawData.substr(0, pos);
                rawData.erase(0, pos + 1);

                // Analyse de la commande
                if (cmd == "L") remote->moveLeft();
                else if (cmd == "R") remote->moveRight();
                else if (cmd == "ROT") remote->rotate();
                else if (cmd == "SOFT") remote->softDrop();
                else if (cmd == "DROP") remote->hardDrop();
                else if (cmd == "HOLD") remote->hold();
                // --- AJOUT ICI ---
                else if (cmd == "FALL") {
                    remote->networkForceUpdate(); // On force la gravité sur la marionnette
                }
                
                // --- AJOUTER ICI LE BLOC ATK ---
                else if (cmd.find("ATK:") == 0) {
                    // On récupère le chiffre après les 4 caractères "ATK:"
                    // Exemple ATK:2 -> on prend "2"
                    try {
                        int lines = std::stoi(cmd.substr(4));
                        // Qui reçoit l'attaque ? C'est MOI (l'opposé du remote)
                        TetrisInstance* me = (net.getRole() == NetworkRole::SERVER) ? player1 : player2;
                        me->addGarbage(lines);
                    } catch (...) {} // Sécurité si le nombre est malformé
                }
                // --- AJOUTER ICI LE BLOC GAMEOVER (voir étape suivante) ---
                else if (cmd == "GAMEOVER") {
                    currentState = GameState::GAME_OVER;
                    // audio.playGameOver();
                }
                // Commande de Restart (Valable en GAME_OVER)
                else if (cmd == "RESTART") {
                    resetGame();
                    currentState = GameState::PLAY;
                }
                // SEED (Valable tout le temps)
                else if (cmd.find("SEED:") == 0) {
                    try {
                        int seed = std::stoi(cmd.substr(5));
                        
                        // 1. On applique la graine synchronisée
                        srand(seed);
                        std::cout << "SEED recu et applique : " << seed << std::endl;

                        // 2. CRUCIAL : On redémarre le jeu MAINTENANT !
                        // Cela va recréer les TetrisInstance, et leurs constructeurs vont appeler rand()
                        // qui utilisera la nouvelle graine synchronisée.
                        resetGame();
                        
                    } catch (...) {
                        std::cout << "Erreur lecture SEED" << std::endl;
                    }
                }
            }
        }
    }

    if (currentState != GameState::PLAY) return;

        if (!isNetworkGame) {
        // Mode Local : On met à jour les deux comme avant
            player1->update();
            player2->update();
        } 
        else {
            // Mode Réseau : On ne met à jour QUE soi-même !
            // L'autre est une marionnette, il ne bouge que si on reçoit des ordres.
            
            TetrisInstance* me = (net.getRole() == NetworkRole::SERVER) ? player1 : player2;
            
            // Si ma pièce tombe naturellement...
            if (me->update()) {
                // ... Je préviens l'autre !
                net.sendData("FALL");
            }
        }

    // --- GESTION DE L'ATTAQUE ---
    int p1Attack = player1->popLinesCleared();
    if (p1Attack > 1) { 
        player2->addGarbage(p1Attack - 1); 
         // Si je suis J1 (Serveur), je préviens l'autre qu'il est attaqué
        if (isNetworkGame && net.getRole() == NetworkRole::SERVER) {
            net.sendData("ATK:" + std::to_string(p1Attack - 1));
        }
    }

    int p2Attack = player2->popLinesCleared();
    if (p2Attack > 1) {
        player1->addGarbage(p2Attack - 1);
        if (isNetworkGame && net.getRole() == NetworkRole::CLIENT) {
            net.sendData("ATK:" + std::to_string(p2Attack - 1));
        }
    }

    // Détection Game Over
    // On vérifie d'abord si NOUS avons perdu localement
    bool serverLost = player1->getGameOver();
    bool clientLost = player2->getGameOver();

    if (serverLost || clientLost) {
        // Si on n'est pas déjà en écran Game Over, on envoie le signal
        if (currentState != GameState::GAME_OVER) {
            currentState = GameState::GAME_OVER;
            
            // Si on est en réseau, on prévient l'autre qu'il y a un mort
            if (isNetworkGame) {
                net.sendData("GAMEOVER");
            }
        }
    }

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 30, 255);
    SDL_RenderClear(renderer);
    
    // On définit quels états doivent afficher le plateau de jeu
    bool showGame = (currentState == GameState::PLAY || 
                    currentState == GameState::PAUSE || 
                    currentState == GameState::GAME_OVER);

    // Si on est dans un état de jeu, on affiche les grilles
    if (showGame) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

        if (player1) player1->draw();
        if (player2) player2->draw();

        renderControlsHelp();
    }

    // Affichage des interfaces (Overlays)
    switch (currentState) {
        case GameState::MENU:      renderMenu(); break;
        case GameState::PAUSE:     renderPause(); break;
        case GameState::GAME_OVER: renderGameOver(); break;
        case GameState::MODE_SELECTION: renderModeSelection(); break;
        case GameState::MULTI_HOST:   renderMultiHost(); break;
        case GameState::MULTI_JOIN:   renderMultiJoin(); break;
        default: break;
    }

    SDL_RenderPresent(renderer);
}

Game::~Game() {
    delete player1;
    delete player2;
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void Game::resetGame() {
    // Nettoyage sécurisé
    if (player1) { delete player1; player1 = nullptr; }
    if (player2) { delete player2; player2 = nullptr; }

    // --- AJOUT ICI ---
    // On s'assure que la musique repart quand on lance une nouvelle partie
    audio.playMusic(); 
    // -----------------

    // Création de nouveaux joueurs
    player1 = new TetrisInstance(renderer, font, &audio, 140, 50);
    player2 = new TetrisInstance(renderer, font, &audio, 700, 50);
}


// --- RENDERING HELPERS ---

void Game::drawCenteredText(std::string text, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = { (SCREEN_WIDTH - surf->w) / 2, y, surf->w, surf->h }; // Centré X
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void Game::renderMenu() {
    // Fond simple ou image si tu veux
    drawCenteredText("TETRIS VERSUS", 200, {0, 255, 255, 255});
    drawCenteredText("Appuyez sur ENTREE pour jouer", 400, {255, 255, 255, 255});
    drawCenteredText("J1: ZQSD + Espace | J2: Fleches + Entree", 500, {150, 150, 150, 255});

    drawCenteredText("ECHAP : Quitter le jeu", 600, {100, 100, 100, 255}); // En gris sombre
}

void Game::renderPause() {
    // Fond semi-transparent noir
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawCenteredText("PAUSE", 300, {255, 255, 0, 255});
    drawCenteredText("P: Reprendre  |  Q: Menu", 400, {255, 255, 255, 255});
}

void Game::renderGameOver() {
    // Fond semi-transparent rouge
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 200);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    std::string winner;
    if (player1->getGameOver() && !player2->getGameOver()) winner = "JOUEUR 2 GAGNE !";
    else if (!player1->getGameOver() && player2->getGameOver()) winner = "JOUEUR 1 GAGNE !";
    else winner = "EGALITE !";

    drawCenteredText("GAME OVER", 250, {255, 0, 0, 255});
    drawCenteredText(winner, 320, {0, 255, 0, 255});
    drawCenteredText("ESPACE: Rejouer  |  ECHAP: Menu", 500, {255, 255, 255, 255});
}

// void Game::renderMultiChoice() {
//     drawCenteredText("MODE MULTIJOUEUR", 200, {0, 255, 255, 255});
//     drawCenteredText("H: Heberger une partie (Host)", 350, {255, 255, 255, 255});
//     drawCenteredText("J: Rejoindre une partie (Join)", 450, {255, 255, 255, 255});
// }

void Game::renderMultiHost() {
    drawCenteredText("SALLE D'ATTENTE", 150, {0, 255, 0, 255});
    
    std::string ipMsg = "CODE DE LA PARTIE : " + net.getLocalIP();
    drawCenteredText(ipMsg, 300, {255, 255, 0, 255});
    
    drawCenteredText("Communiquez ce code au joueur 2.", 350, {200, 200, 200, 255});
    drawCenteredText("En attente de connexion...", 500, {255, 255, 255, 255});

    drawCenteredText("ECHAP: Retour", 550, {150, 150, 150, 255});
}

void Game::renderMultiJoin() {
     drawCenteredText("REJOINDRE UNE PARTIE", 150, {0, 255, 255, 255});
    drawCenteredText("Entrez le Code Partie :", 300, {255, 255, 255, 255});
    
    // Zone de saisie
    SDL_Rect box = { (SCREEN_WIDTH/2) - 150, 340, 300, 40 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &box);
    
    drawCenteredText(inputText + "_", 345, {0, 255, 0, 255});
    
    drawCenteredText("Appuyez sur ENTREE pour valider", 500, {150, 150, 150, 255});

    drawCenteredText("ECHAP: Retour", 550, {150, 150, 150, 255});

    // --- AJOUT : Affichage du message d'erreur/statut ---
    if (!connectionStatus.empty()) {
        drawCenteredText(connectionStatus, 420, {255, 100, 100, 255}); // En rouge clair
    }
}

void Game::renderModeSelection() { // Remplace renderMultiChoice
    drawCenteredText("CHOIX DU MODE DE JEU", 150, {0, 255, 255, 255});
    
    drawCenteredText("L: Versus Local (Meme PC)", 300, {255, 255, 0, 255}); // Jaune pour le local
    
    drawCenteredText("H: Heberger en Reseau (Host)", 400, {255, 255, 255, 255});
    drawCenteredText("J: Rejoindre en Reseau (Join)", 450, {255, 255, 255, 255});
    
    drawCenteredText("ECHAP: Retour", 550, {150, 150, 150, 255});
}


void Game::renderControlsHelp() {
    std::string msg;
    SDL_Color color = {150, 150, 150, 255}; // Gris clair

    if (!isNetworkGame) {
        // Mode Local
        msg = "J1 (Gauche): ZQSD + Espace  |  J2 (Droite): Fleches + Entree";
    } 
    else {
        // Mode Réseau
        if (net.getRole() == NetworkRole::SERVER) {
            msg = "VOUS ETES J1 (HOTE) >>> Utilisez ZQSD + ESPACE";
            color = {0, 255, 255, 255}; // Cyan pour qu'il repère bien son côté
        } 
        else {
            msg = "VOUS ETES J2 (INVITE) >>> Utilisez FLECHES + ENTREE";
            color = {0, 255, 255, 255};
        }
    }

    // Afficher tout en bas de l'écran
    drawCenteredText(msg, SCREEN_HEIGHT - 30, color);
}


