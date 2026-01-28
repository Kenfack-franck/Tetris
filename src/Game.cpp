#include "../include/Game.hpp"
#include "../include/Defs.hpp"
#include <iostream>
#include <ctime>

Game::Game(SDL_Renderer* ren) 
    : renderer(ren), 
      isGameRunning(true),
      currentState(GameState::MENU), // On commence au MENU
      player1(nullptr),              // IMPORTANT
      player2(nullptr),              // IMPORTANT
      bgMenuTexture(nullptr),
      avatar1Texture(nullptr),
      avatar2Texture(nullptr)
{
    std::cout << "[DEBUG] Game constructor START" << std::endl;
    
    // 1. D'ABORD : Initialiser la librairie de texte
    std::cout << "[DEBUG] Initializing TTF..." << std::endl;
    if (TTF_Init() == -1) {
        std::cout << "ERREUR CRITIQUE : TTF_Init n'a pas pu demarrer ! " << TTF_GetError() << std::endl;
    }

    // Lancer la musique au démarrage
    std::cout << "[DEBUG] Starting music..." << std::endl;
    audio.playMusic();

    // 2. ENSUITE : Charger la police
    std::cout << "[DEBUG] Loading font..." << std::endl;
    font = TTF_OpenFont("assets/font.ttf", 24);

    if (!font) {
        std::cout << "ERREUR : Impossible de charger assets/font.ttf ! " << TTF_GetError() << std::endl;
        std::cout << "Essai de chargement de Arial par defaut..." << std::endl;
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }

    if (font) {
        std::cout << "Succes : Police chargee !" << std::endl;
    }

    // 3. Charger les images (BMP supporté nativement, PNG nécessite SDL2_image)
    std::cout << "[DEBUG] Loading background image..." << std::endl;
    bgMenuTexture = loadImage("assets/menu_bg.bmp");
    if (!bgMenuTexture) {
        std::cout << "[INFO] Pas d'image de fond - utilisera le dégradé" << std::endl;
    }
    
    std::cout << "[DEBUG] Loading avatar images..." << std::endl;
    avatar1Texture = loadImage("assets/avatar1.bmp");
    avatar2Texture = loadImage("assets/avatar2.bmp");
    if (!avatar1Texture) {
        std::cout << "[INFO] Avatar 1 non trouvé" << std::endl;
    }
    if (!avatar2Texture) {
        std::cout << "[INFO] Avatar 2 non trouvé" << std::endl;
    }
    
    std::cout << "[DEBUG] Initializing network..." << std::endl;
    // Init Réseau
    if (!net.init()) {
        std::cout << "Erreur Init Reseau !" << std::endl;
    }

    std::cout << "[DEBUG] Calling resetGame()..." << std::endl;
    resetGame();
    
    std::cout << "[DEBUG] Game constructor DONE" << std::endl;
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
                std::cout << "[DEBUG] Appui sur L - Mode LOCAL" << std::endl;
                isNetworkGame = false;
                std::cout << "[DEBUG] Avant resetGame()" << std::endl;
                resetGame(); // Prépare les 2 joueurs locaux
                std::cout << "[DEBUG] Après resetGame()" << std::endl;
                currentState = GameState::PLAY;
                std::cout << "[DEBUG] État changé à PLAY" << std::endl;
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
            // Utiliser .get() pour obtenir le pointeur brut depuis unique_ptr
            TetrisInstance* remote = (net.getRole() == NetworkRole::SERVER) ? player2.get() : player1.get();

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
                        // Utiliser .get() pour obtenir le pointeur brut depuis unique_ptr
                        TetrisInstance* me = (net.getRole() == NetworkRole::SERVER) ? player1.get() : player2.get();
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
            
            // Utiliser .get() pour obtenir le pointeur brut depuis unique_ptr
            TetrisInstance* me = (net.getRole() == NetworkRole::SERVER) ? player1.get() : player2.get();
            
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
    static int renderCount = 0;
    if (renderCount % 120 == 0) {  // Log tous les 120 frames (~2 sec)
        std::cout << "[DEBUG RENDER] Frame " << renderCount << ", state=" << (int)currentState << std::endl;
    }
    renderCount++;
    
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

        // Afficher les avatars des joueurs en haut de chaque côté avec labels
        auto drawLabel = [&](std::string txt, int x, int y, SDL_Color col) {
            if(!font) return;
            SDL_Surface* surf = TTF_RenderUTF8_Solid(font, txt.c_str(), col);
            if(surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {x, y, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        };

        if (avatar1Texture) {
            drawAvatarImage(avatar1Texture, 15, 10, 90);  // Player 1 top left - larger
            drawLabel("JOUEUR 1", 115, 15, {0, 200, 255, 255});  // Label to the right - moved up
        }
        if (avatar2Texture) {
            drawAvatarImage(avatar2Texture, 1110, 10, 90);  // Player 2 top right - larger and more to the right
            drawLabel("JOUEUR 2", 985, 15, {255, 100, 200, 255});  // Label to the left - moved up
        }

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
    // Les unique_ptr sont détruits automatiquement - pas besoin de delete!
    // player1 et player2 sont automatiquement détruits quand Game meurt
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void Game::resetGame() {
    std::cout << "[DEBUG] resetGame() START" << std::endl;
    
    // Les anciens joueurs (player1 et player2) sont détruits AUTOMATIQUEMENT
    // quand on les reassigne avec make_unique (les unique_ptr gèrent la destruction)
    // Pas besoin de if/delete comme avant!

    // --- AJOUT ICI ---
    // On s'assure que la musique repart quand on lance une nouvelle partie
    audio.playMusic(); 
    // -----------------

    // Création de nouveaux joueurs avec make_unique (allocation gérée automatiquement)
    std::cout << "[DEBUG] Creating player1 at (140, 50)..." << std::endl;
    player1 = std::make_unique<TetrisInstance>(renderer, font, &audio, 140, 50);
    std::cout << "[DEBUG] Player1 created successfully" << std::endl;
    
    std::cout << "[DEBUG] Creating player2 at (700, 50)..." << std::endl;
    player2 = std::make_unique<TetrisInstance>(renderer, font, &audio, 700, 50);
    std::cout << "[DEBUG] Player2 created successfully" << std::endl;
    
    std::cout << "[DEBUG] resetGame() DONE" << std::endl;
}


// --- RENDERING HELPERS ---

SDL_Texture* Game::loadImage(const std::string& path) {
    std::cout << "[DEBUG] Tentative de chargement image: " << path << std::endl;
    
    // Charger via SDL_LoadBMP si c'est un BMP
    if (path.find(".bmp") != std::string::npos) {
        SDL_Surface* surf = SDL_LoadBMP(path.c_str());
        if (!surf) {
            std::cout << "[WARN] Impossible de charger le BMP " << path << ": " << SDL_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (tex) {
            std::cout << "[DEBUG] Image BMP chargée avec succès: " << path << std::endl;
        } else {
            std::cout << "[WARN] Impossible de créer la texture: " << SDL_GetError() << std::endl;
        }
        return tex;
    }
    
    // Pour PNG/JPG/etc - utilise SDL2_image s'il était disponible
    std::cout << "[WARN] Format PNG non supporté sans SDL2_image. Utilisez BMP ou installez SDL2_image." << std::endl;
    return nullptr;
}

void Game::drawImageBackground(SDL_Texture* bgTex) {
    if (!bgTex) return;
    SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, bgTex, NULL, &dst);
}

void Game::drawGradientBG() {
    // Dégradé bleu noir - simple et efficace
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int blue = (y * 100) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, 10, 10, 30 + blue, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}

void Game::drawCenteredText(std::string text, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surf = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = { (SCREEN_WIDTH - surf->w) / 2, y, surf->w, surf->h };
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void Game::drawAvatar(int x, int y, int size, SDL_Color color) {
    // Avatar simple = rectangle avec décoration
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_Rect avatar = {x, y, size, size};
    SDL_RenderFillRect(renderer, &avatar);
    
    // Bordure highlight
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
    SDL_RenderDrawLine(renderer, x, y, x + size, y);
    SDL_RenderDrawLine(renderer, x, y, x, y + size);
    
    // Bordure shadow
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderDrawLine(renderer, x + size, y, x + size, y + size);
    SDL_RenderDrawLine(renderer, x, y + size, x + size, y + size);
}

void Game::drawAvatarImage(SDL_Texture* avatarTex, int x, int y, int size) {
    if (!avatarTex) return;
    SDL_Rect dst = {x, y, size, size};
    SDL_RenderCopy(renderer, avatarTex, NULL, &dst);
}

void Game::renderMenu() {
    // Afficher image de fond si disponible, sinon dégradé
    if (bgMenuTexture) {
        drawImageBackground(bgMenuTexture);
        // Overlay semi-transparent pour meilleure lisibilité du texte
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    } else {
        drawGradientBG();
    }

    // ===== TITRE EN HAUT =====
    drawCenteredText("TETRIS VERSUS", 40, {0, 255, 255, 255});
    drawCenteredText("━━━━━━━━━━━━━━━━━━━━━━━━", 85, {0, 200, 255, 200});

    // ===== ZONE JOUEURS CÔTE À CÔTE =====
    // J1 à gauche
    if (avatar1Texture) {
        drawAvatarImage(avatar1Texture, 150, 150, 80);
    } else {
        drawAvatar(150, 150, 80, {0, 255, 255});
    }

    // J2 à droite
    if (avatar2Texture) {
        drawAvatarImage(avatar2Texture, 950, 150, 80);
    } else {
        drawAvatar(950, 150, 80, {255, 100, 255});
    }

    // ===== INSTRUCTIONS CENTRALES =====
    // Instruction pulsante (animation simple)
    static int pulse = 0;
    pulse = (SDL_GetTicks() / 500) % 2;
    std::string pressText = pulse ? ">>> APPUYEZ SUR ENTREE <<<" : "    APPUYEZ SUR ENTREE    ";
    drawCenteredText(pressText, 400, {100, 255, 100, 255});

    // Sous-texte
    drawCenteredText("★ Défiez votre ami pour la victoire ! ★", 450, {255, 200, 100, 255});

    // ===== BAS =====
    drawCenteredText("━━━━━━━━━━━━━━━━━━━━━━━━", 520, {100, 150, 255, 150});
    drawCenteredText("[ESC] Quitter", 570, {200, 100, 100, 255});
}

void Game::renderPause() {
    // Fond semi-transparent noir avec dégradé
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Boîte décorée
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
    SDL_Rect pauseBox = {SCREEN_WIDTH/2 - 200, 250, 400, 200};
    SDL_RenderDrawRect(renderer, &pauseBox);

    drawCenteredText("━━━━━━━━━━━━━━", 270, {100, 255, 255, 255});
    drawCenteredText("⏸ PAUSE", 300, {255, 255, 100, 255});
    drawCenteredText("━━━━━━━━━━━━━━", 330, {100, 255, 255, 255});
    drawCenteredText("[P] Reprendre", 370, {200, 255, 200, 255});
    drawCenteredText("[Q] Retour Menu", 410, {255, 150, 150, 255});
}

void Game::renderGameOver() {
    // Fond semi-transparent rouge
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 200);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    std::string winner;
    if (player1->getGameOver() && !player2->getGameOver()) winner = "🏆 JOUEUR 2 GAGNE ! 🏆";
    else if (!player1->getGameOver() && player2->getGameOver()) winner = "🏆 JOUEUR 1 GAGNE ! 🏆";
    else winner = "☆ EGALITE ! ☆";

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
    // Fond
    drawImageBackground(bgMenuTexture);
    
    // Voile semi-transparent pour floudifier (assombrir un peu)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100); // Noir avec 100/255 d'opacité (40%)
    SDL_RenderFillRect(renderer, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    drawCenteredText("CHOIX DU MODE DE JEU", 150, {0, 255, 255, 255});
    
    drawCenteredText("L: Versus Local (Meme PC)", 300, {255, 255, 0, 255}); // Jaune pour le local
    
    drawCenteredText("H: Heberger en Reseau (Host)", 400, {255, 255, 255, 255});
    drawCenteredText("J: Rejoindre en Reseau (Join)", 450, {255, 255, 255, 255});
    
    drawCenteredText("ECHAP: Retour", 550, {255, 255, 255, 255});
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


