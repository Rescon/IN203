#include <cstdlib>
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
#include <ctime>
#include <iomanip>      // std::setw
#include <chrono>
#include <mpi.h>

#include "parametres.hpp"
#include "galaxie.hpp"
 
int main(int argc, char ** argv)
{
    int width, height, height_pas;
    parametres param;

    // Initialiser MPI
    MPI_Init(&argc, &argv);
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    int nbp;
    MPI_Comm_size(globComm, &nbp);
    int rank;
    MPI_Comm_rank(globComm, &rank);
    MPI_Status status;
    
    // Lecture des paramètres
    char commentaire[4096];
    std::ifstream fich("parametre.txt");
    fich >> width;
    fich.getline(commentaire, 4096);
    fich >> height;
    fich.getline(commentaire, 4096);
    fich >> param.apparition_civ;
    fich.getline(commentaire, 4096);
    fich >> param.disparition;
    fich.getline(commentaire, 4096);
    fich >> param.expansion;
    fich.getline(commentaire, 4096);
    fich >> param.inhabitable;
    fich.getline(commentaire, 4096);
    fich.close();
    
    // Diviser le graphique entier en parties nbp-1
    height_pas = int(height/(nbp-1));
    
    if (rank == 0){
        //Le processus 0 est le processus principal, responsable de la réception des résultats calculés, de l'affichage et l'envoi des "fantômes cellules"
        
        // Sortie des paramètres importants
        std::cout << "Resume des parametres (proba par pas de temps): " << std::endl;
        std::cout << "\t Chance apparition civilisation techno : " << param.apparition_civ << std::endl;
        std::cout << "\t Chance disparition civilisation techno: " << param.disparition << std::endl;
        std::cout << "\t Chance expansion : " << param.expansion << std::endl;
        std::cout << "\t Chance inhabitable : " << param.inhabitable << std::endl;
        std::cout << "Proba minimale prise en compte : " << 1./RAND_MAX << std::endl;
        std::srand(std::time(nullptr));
        
        
        // Initialiser SDL
        SDL_Event event;
        SDL_Window   * window;
        SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      width, height, SDL_WINDOW_SHOWN);
        
        // Sortie de pas de temps
        int deltaT = (20*52840)/width;
        std::cout << "Pas de temps : " << deltaT << " années" << std::endl;
        std::cout << std::endl;
        unsigned long long temps = 0;
        std::chrono::time_point<std::chrono::system_clock> start, end1, end2;
		
        // Créer de graphique initial et l'afficher
        galaxie g(width, height, param.apparition_civ);
        galaxie_renderer gr(window);
        gr.render(g);
        
        // Créer un tampon de réception, un tampon d'envoi et un tampon pour envoyer les “fantômes cellules”
        std::vector<char> send_buff(width*height_pas);
        std::vector<char> recv_buff(width*height_pas);
        std::vector<char> send_buff_bord(width);
            
        // Diviser le graphique initial et envoyez-les au processus correspondant
        for(int count = 1; count < nbp; ++count){
            for(int i = 0; i < height_pas; ++i){
                for(int j = 0; j < width; ++j){
                    send_buff[i*width+j] = *(g.data()+(count-1)*width*height_pas+i*width+j);
                }
            }
            MPI_Send(send_buff.data(), width*height_pas, MPI_CHAR, count, 0, globComm);
        }
        
        // Jusqu'à présent, l'initialisation de chaque processus est terminée, puis ils peuvent être bouclée jusqu'à ce que l'utilisateur ferme la fenêtre (termine le programme)
        while(1){
            
            // Envoyer les “fantômes cellules” située sous les blocs à processus 1, ..., nbp-2
        	start = std::chrono::system_clock::now();
        	for(int count = 1; count < nbp-1; ++count){
                for(int j = 0; j < width; ++j){
                    send_buff_bord[j] = *(g.data()+count*width*height_pas+j);
                }
                MPI_Send(send_buff_bord.data(), width, MPI_CHAR, count, 0, globComm);
            }
            
            // Envoyer les “fantômes cellules” située sur les blocs à processus 2, ..., nbp-1
            for(int count = 2; count < nbp; ++count){
                for(int j = 0; j < width; ++j){
                    send_buff_bord[j] = *(g.data()+(count-1)*width*height_pas-width+j);   
                }
                MPI_Send(send_buff_bord.data(), width, MPI_CHAR, count, 0, globComm);
            }
            
            // Obtenir les résultats du calcul de chaque processus et les mettre à jour dans la position correspondante
            for(int count = 1; count < nbp; ++count){
                MPI_Recv(recv_buff.data(), width*height_pas, MPI_CHAR, count, 0, globComm, &status);
                g.SetValue(recv_buff, (count-1)*width*height_pas, height_pas);
            }
            
            
            end1 = std::chrono::system_clock::now();
            
            // Affichage.
            gr.render(g);
            
            end2 = std::chrono::system_clock::now();

            std::chrono::duration<double> elaps1 = end1 - start;
            std::chrono::duration<double> elaps2 = end2 - end1;
                
            temps += deltaT;
            std::cout << "Temps passe : "
                      << std::setw(10) << temps << " années"
                      << std::fixed << std::setprecision(3)
                      << "  " << "|  CPU(ms) : calcul " << elaps1.count()*1000
                      << "  " << "affichage " << elaps2.count()*1000
                      << "\r" << std::flush;
            //_sleep(1000);
             
            // Vérifier si l'utilisateur ferme la fenêtre (termine le programme)
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
                std::cout << std::endl << "The end" << std::endl;
                break;
            }
        }
        
        SDL_DestroyWindow(window);
        SDL_Quit();
        
    }else if(rank == 1){
        
        // Pour le processus 1, une seule ligne de "cellules fantômes" est nécessaire
        std::vector<char> send_buff(width*height_pas);
        std::vector<char> recv_buff(width*height_pas);
        std::vector<char> recv_buff_bord(width);
        std::vector<char> g(width*height_pas);
        std::vector<char> g_next(width*height_pas);
        MPI_Recv(recv_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm, &status);
		g.assign(recv_buff.begin(), recv_buff.end());
        // Jusqu'à présent, l'initialisation de processus 1 est terminée
        
        while(1){

            // Recevoir les “fantômes cellules” nécessaires et les combiner avec la zone de calcul
        	MPI_Recv(recv_buff_bord.data(), width, MPI_CHAR, 0, 0, globComm, &status);
            g.insert(g.end(), recv_buff_bord.begin(), recv_buff_bord.end());
            
            // Mettre à jour la zone de calcul
            mise_a_jour(param, width, height_pas+1, g.data(), g_next.data());

            // Diviser les résultats requis et envoyer-les au processus principal
            send_buff.assign(g_next.begin(), g_next.end()-width);
            g.assign(send_buff.begin(), send_buff.end());
            MPI_Send (send_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm);
        }
    }else if(rank == nbp-1){
        
        // Pour le processus nbp-1, une seule ligne de "cellules fantômes" est nécessaire
    	std::vector<char> send_buff(width*height_pas);
        std::vector<char> recv_buff(width*height_pas);
        std::vector<char> recv_buff_bord(width);
        std::vector<char> g(width*height_pas);
        std::vector<char> g_next(width*height_pas);
        MPI_Recv(recv_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm, &status);
        g.assign(recv_buff.begin(), recv_buff.end());
        // Jusqu'à présent, l'initialisation de processus nbp-1 est terminée
        
        while(1){
            
            // Recevoir les “fantômes cellules” nécessaires et les combiner avec la zone de calcul
        	MPI_Recv(recv_buff_bord.data(), width, MPI_CHAR, 0, 0, globComm, &status);
            g.insert(g.begin(), recv_buff_bord.begin(), recv_buff_bord.end());
            
            // Mettre à jour la zone de calcul
            mise_a_jour(param, width, height_pas+1, g.data(), g_next.data());
            
            // Diviser les résultats requis et envoyer-les au processus principal
            send_buff.assign(g_next.begin()+width, g_next.end());
            g.assign(send_buff.begin(), send_buff.end());
            MPI_Send (send_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm);
        }
    }else{
        
        // Pour les autres processus, deux lignes de "cellules fantômes" sont nécessaires
    	std::vector<char> send_buff(width*height_pas);
        std::vector<char> recv_buff(width*height_pas);
        std::vector<char> recv_buff_bord(width);
        std::vector<char> g(width*height_pas);
        std::vector<char> g_next(width*height_pas);
        MPI_Recv(recv_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm, &status);
        g.assign(recv_buff.begin(), recv_buff.end());
        // Jusqu'à présent, l'initialisation de chaque processus est terminée
        
        while(1){
            
            // Recevoir les “fantômes cellules” nécessaires et les combiner avec la zone de calcul
        	MPI_Recv(recv_buff_bord.data(), width, MPI_CHAR, 0, 0, globComm, &status);
            g.insert(g.end(), recv_buff_bord.begin(), recv_buff_bord.end());
            MPI_Recv(recv_buff_bord.data(), width, MPI_CHAR, 0, 0, globComm, &status);
            g.insert(g.begin(), recv_buff_bord.begin(), recv_buff_bord.end());
            
            // Mettre à jour la zone de calcul
            mise_a_jour(param, width, height_pas+2, g.data(), g_next.data());
            
            // Diviser les résultats requis et envoyer-les au processus principal
            send_buff.assign(g_next.begin()+width, g_next.end()-width);
            g.assign(send_buff.begin(), send_buff.end());
            MPI_Send (send_buff.data(), width*height_pas, MPI_CHAR, 0, 0, globComm);
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
