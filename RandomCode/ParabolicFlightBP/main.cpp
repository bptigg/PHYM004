#include <armadillo>

int main()
{
    int timesteps = 10;
    double MaxTime = 1.0;
    double MaxHeight = 10;
    double g = 9.81;
    double Timestep = MaxTime / (double)timesteps;
    arma::mat A = arma::mat(timesteps + 1, timesteps + 1, arma::fill::zeros);

    A(0,0) = 1.0;
    A(timesteps, timesteps) = 1.0;

    for(int i = 1; i < timesteps; i++)
    {
        A(i,i-1) = 1.0;
        A(i,i) = -2.0;
        A(i,i+1) = 1.0;
    }

    arma::vec b = arma::vec(timesteps+1, arma::fill::zeros);
    b(0) = 0;
    b(timesteps)  = MaxHeight;
    for(int i = 1; i < timesteps; i++)
    {
        b(i) = -1.0 * g * Timestep;
    }
    
    std::cout << A << std::endl;
    std::cout << b << std::endl;
    
}