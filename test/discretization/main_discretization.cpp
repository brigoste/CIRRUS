#include <iostream>

void runGreenGaussTest();
void runLeastSquaresTest();
void runCubicGradientTest();
void runGradientReconstructionTest();

int main()
{
    std::cout << "==============================\n";
    std::cout << " Discretization Tests\n";
    std::cout << "==============================\n\n";

    std::cout << "---- Linear Gradient Tests ----\n\n";
    runGreenGaussTest();
    runLeastSquaresTest();
    std::cout << "\n";
    
    std::cout << "---- Gradient Verification Tests ----\n\n";
    runCubicGradientTest();

    std::cout << "---- Gradient Reconstruction Tests ----\n\n";
    runGradientReconstructionTest();

    std::cout << "\n==============================\n";
    std::cout << " COMPLETE\n";
    std::cout << "==============================\n";

    return 0;
}
