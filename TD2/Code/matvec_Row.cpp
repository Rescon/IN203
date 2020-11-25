// Produit matrice-vecteur
# include <cassert>
# include <vector>
# include <iostream>
# include <mpi.h>

// ---------------------------------------------------------------------
class Matrix : public std::vector<double>
{
public:
    Matrix (int dim);
    Matrix( int nrows, int ncols );
    Matrix( const Matrix& A ) = delete;
    Matrix( Matrix&& A ) = default;
    ~Matrix() = default;

    Matrix& operator = ( const Matrix& A ) = delete;
    Matrix& operator = ( Matrix&& A ) = default;
    
    double& operator () ( int i, int j ) {
        return m_arr_coefs[i + j*m_nrows];
    }
    double  operator () ( int i, int j ) const {
        return m_arr_coefs[i + j*m_nrows];
    }
    
    std::vector<double> operator * ( const std::vector<double>& u ) const;
    
    std::ostream& print( std::ostream& out ) const
    {
        const Matrix& A = *this;
        out << "[\n";
        for ( int i = 0; i < m_nrows; ++i ) {
            out << " [ ";
            for ( int j = 0; j < m_ncols; ++j ) {
                out << A(i,j) << " ";
            }
            out << " ]\n";
        }
        out << "]";
        return out;
    }
private:
    int m_nrows, m_ncols;
    std::vector<double> m_arr_coefs;
};
// ---------------------------------------------------------------------
inline std::ostream& 
operator << ( std::ostream& out, const Matrix& A )
{
    return A.print(out);
}
// ---------------------------------------------------------------------
inline std::ostream&
operator << ( std::ostream& out, const std::vector<double>& u )
{
    out << "[ ";
    for ( const auto& x : u )
        out << x << " ";
    out << " ]";
    return out;
}
// ---------------------------------------------------------------------
std::vector<double> 
Matrix::operator * ( const std::vector<double>& u ) const
{
    const Matrix& A = *this;
    assert( u.size() == unsigned(m_ncols) );
    std::vector<double> v(m_nrows, 0.);
    for ( int i = 0; i < m_nrows; ++i ) {
        for ( int j = 0; j < m_ncols; ++j ) {
            v[i] += A(i,j)*u[j];
        }            
    }
    return v;
}

// =====================================================================
Matrix::Matrix (int dim) : m_nrows(dim), m_ncols(dim),
                           m_arr_coefs(dim*dim)
{
    for ( int i = 0; i < dim; ++ i ) {
        for ( int j = 0; j < dim; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }
}
// ---------------------------------------------------------------------
Matrix::Matrix( int nrows, int ncols ) : m_nrows(nrows), m_ncols(ncols),
                                         m_arr_coefs(nrows*ncols)
{
    int dim = (nrows > ncols ? nrows : ncols );
    for ( int i = 0; i < nrows; ++ i ) {
        for ( int j = 0; j < ncols; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }    
}
// =====================================================================
int main( int nargs, char* argv[] )
{
    const int N = 120;
    Matrix A(N);
    std::vector<double> u( N );
    for ( int i = 0; i < N; ++i ) u[i] = i+1;
    std::vector<double> Result_All(N);

    MPI_Init(&nargs,&argv);
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD,&globComm);

    int rank;
    MPI_Comm_rank(globComm,&rank);

    int nbp;
    MPI_Comm_size(globComm,&nbp);
    
    int  Nloc = N / nbp;

    Matrix A_Parti(Nloc,N);
    std::vector<double> Result_Parti(Nloc);
    std::vector<int> recvbuf(Nloc*N);
    std::vector<int> sendbuf(N*N);
    
    // Charger la matrice A à sendbuf.
    if(rank == 0){
        for(int i=0;i<N;++i){
            for(int j=0;j<N;++j){
                sendbuf.at(i*N+j)=A(i,j);
            }
        }
    }
    
    // Disperser les données sur chaque processus.
    MPI_Scatter(sendbuf.data(), Nloc*N, MPI_INT, recvbuf.data(), Nloc*N, MPI_INT, 0, globComm);

    // Charger recvbuf à la matrice A_Parti
    for(int i = 0;i < Nloc;i++){
        for(int j = 0;j < N;j++){
            A_Parti(i,j) = recvbuf.at(i*N+j);
        }
    }

    // Renvoyer le résultat de chaque processus au processus 0
    Result_Parti = A_Parti * u;
    MPI_Gather(Result_Parti.data(), Result_Parti.size(), MPI_DOUBLE, Result_All.data(), Result_Parti.size(), MPI_DOUBLE, 0, globComm);
       
    if(rank == 0){
        std::cout << "A.u = " << Result_All << std::endl;
        // Vérifier l'exactitude des résultats.
        /*
        std::vector<double> Result_True(N);
        Result_True = A * u;
        if(Result_All == Result_True){
            std::cout<<"the result is correct"<<std::endl;
        }
        */
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
