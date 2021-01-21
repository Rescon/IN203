#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
# include <mpi.h>
#include "lodepng_old/lodepng.h"

int main(int nargs, char* argv[])
{
    int range; // Degré voisinage (par défaut 1)
    int nb_cases; // 2^(2*range+1)
    std::int64_t dim; // Nombre de cellules prises pour la simultation
    std::int64_t nb_iter;

    range = 1;
    if ( nargs > 1 ) range = std::stoi(argv[1]);

    nb_cases = (1<<(2*range+1));

    dim = 1000;
    if ( nargs > 2 ) dim = std::stoll(argv[2]);
    nb_iter = dim;
    if ( nargs > 3 ) nb_iter = std::stoll(argv[3]);

    std::cout << "Resume des parametres : " << std::flush << std::endl;
    std::cout << "\tNombre de cellules  : " << dim << std::flush << std::endl;
    std::cout << "\tNombre d'iterations : " << nb_iter << std::flush << std::endl;
    std::cout << "\tDegre de voisinage  : " << range << std::flush << std::endl;
    std::cout << "\tNombre de cas       : " << (1ULL<<nb_cases) << std::flush << std::endl << std::endl;

    double chrono_calcul = 0.;
    double chrono_png    = 0.;

    MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);
	MPI_Status status;

	int dim_pas = int(dim/(nbp-1));
	int nb_cells_loc = dim_pas+2*range;
	int nb_cells = dim+2*range;

    for ( std::int64_t num_config = 0; num_config < (1LL<<nb_cases); ++num_config)
    {
    	if (rank == 0){
	        auto start = std::chrono::steady_clock::now();

	        std::vector<int> cells(nb_iter*nb_cells, 0);
	        cells[nb_cells/2] = 1;

	        for ( std::int64_t iter = 1; iter < nb_iter; ++iter ){
		        std::vector<int> send_buff(nb_cells_loc);
		        for(int count = 1; count < nbp; ++count){
		        	send_buff.assign(cells.begin() + iter*nb_cells + (count-1)*nb_cells_loc, cells.begin() + iter*nb_cells + count*nb_cells_loc);
		            MPI_Send(send_buff.data(), nb_cells_loc, MPI_INT, count, 0, globComm);
		        }

		        std::vector<int> recv_buff(dim_pas);
		        for(int count = 1; count < nbp; ++count){       	
		            MPI_Recv(recv_buff.data(), dim_pas, MPI_INT, count, 0, globComm, &status);
		            for(int i = 0; i < dim_pas;++i){
		            	cells[(iter+1)*nb_cells+1+count*dim_pas+i] = recv_buff[i];
		            }
		        }
		    }

		    auto end = std::chrono::steady_clock::now();    
        	std::chrono::duration<double> elapsed_seconds = end-start;
        	chrono_calcul += elapsed_seconds.count();
        	// Sauvegarde de l'image :
      	 	start = std::chrono::steady_clock::now();
        	std::vector<std::uint8_t> image(4*nb_iter*dim, 0xFF);
        	for ( std::int64_t iter = 0; iter < nb_iter; iter ++ )
        	{
            	for ( std::int64_t i = range; i < range+dim; i ++ )
            	{
                	if (cells[iter*nb_cells + i] == 1) 
                	{
                 	    image[4*iter*dim + 4*(i-range) + 0 ] = 0;
                    	image[4*iter*dim + 4*(i-range) + 1 ] = 0;
                	   	image[4*iter*dim + 4*(i-range) + 2 ] = 0;
                	    image[4*iter*dim + 4*(i-range) + 3 ] = 0xFF;
                	}
            	}
        	}
        	end = std::chrono::steady_clock::now();    
        	char filename[256];
        	sprintf(filename, "config%03lld.png",num_config);
        	lodepng::encode(filename, image.data(), dim, nb_iter);  // ligne a commenter pour supprimer la sauvegarde des fichiers
        	elapsed_seconds = end-start;
        	chrono_png += elapsed_seconds.count();
        }else{

        	for(std::int64_t iter = 1; iter < nb_iter; ++iter ){
        		std::vector<int> recv_buff(nb_cells_loc);
	       		std::vector<int> send_buff(dim_pas);
	        	MPI_Recv(recv_buff.data(), nb_cells_loc, MPI_INT, 0, 0, globComm, &status);
	        	for ( int i = range; i < nb_cells_loc-range; ++i )
	            {
	                int val = 0;
	                for ( int j = i-range; j <= i+range; j++ )
	                {
	                    val = 2*val + recv_buff[j];
	                }
	                val = (1<<val);
	                if ((val&num_config) == 0) 
	                    send_buff[i-range] = 0;
	                else 
	                {
	                    send_buff[i-range] = 1;
	                }
	            }
	            MPI_Send(send_buff.data(), dim_pas, MPI_INT, 0, 0, globComm);
	        }
        }   
    }
    MPI_Finalize();
    std::cout << "Temps mis par le calcul : " << chrono_calcul << " secondes." << std::flush << std::endl;
    std::cout << "Temps mis pour constituer les images : " << chrono_png << " secondes." << std::flush << std::endl;
    return EXIT_SUCCESS;
}
