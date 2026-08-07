#include <iostream>

void runGreenGaussTest();
void runLeastSquaresTest();
void runQuadraticGradientTest();

int main()
{
    std::cout << "==============================\n";
    std::cout << " Discretization Tests\n";
    std::cout << "==============================\n\n";

    std::cout << "---- Linear Gradient Tests ----\n\n";
    runGreenGaussTest();
    runLeastSquaresTest();
    std::cout << "\n";
    
    std::cout << "---- Quadratic Gradient Tests ----\n\n";
    runQuadraticGradientTest();

    std::cout << "\n==============================\n";
    std::cout << " COMPLETE\n";
    std::cout << "==============================\n";

    return 0;
}