#include <iostream>

void runGreenGaussTest();
void runLeastSquaresTest();
void runCubicGradientTest();
void runGradientReconstructionTest();
void runCentralReconstructionTest();
void runSecondOrderUpwindFallbackTest();
void runQuickReconstructionTest();
void runMusclReconstructionTest();

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

    std::cout << "---- Central Reconstruction Test ----\n\n";
    runCentralReconstructionTest();

    std::cout << "---- Second-Order Upwind Fallback Test ----\n\n";
    runSecondOrderUpwindFallbackTest();

    std::cout << "\n---- QUICK Reconstruction Test ----\n\n";
    runQuickReconstructionTest();

    std::cout << "\n---- MUSCL Reconstruction Test ----\n\n";
    runMusclReconstructionTest();

    std::cout << "\n==============================\n";
    std::cout << " COMPLETE\n";
    std::cout << "==============================\n";

    return 0;
}
