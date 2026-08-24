#include <iostream>

void runGreenGaussTest();
void runLeastSquaresTest();
void runCubicGradientTest();

void runGradientReconstructionTest();
void runCentralReconstructionTest();
void runSecondOrderUpwindFallbackTest();
void runQuickReconstructionTest();
void runMusclReconstructionTest();
void runMinmodLimiterTest();
void runTVDReconstructionTest();

void runInterpolatorTest();
void runVectorInterpolatorTest();
void runBilinearInterpolatorTest();

void runPointValueQoITest();

void testSSORApplication();
void testSSORInvalidOmega();
void testSSORZeroDiagonal();
void testSSORSGSEquivalence();
void testSSORNonsymmetric();

void runFluxLimiterTest();

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

    std::cout << "\n===================================\n Misc. Reconstructor Checks \n===================================\n\n";

    std::cout << "---- Second-Order Upwind Fallback Test ----\n\n";
    runSecondOrderUpwindFallbackTest();

    std::cout << "\n---- QUICK Reconstruction Test ----\n\n";
    runQuickReconstructionTest();

    std::cout << "\n---- MUSCL Reconstruction Test ----\n\n";
    runMusclReconstructionTest();

    std::cout << "\n---- Minmod Limiter Test ----\n\n";
    runMinmodLimiterTest();

    std::cout << "\n---- TVD Reconstruction Test ----\n\n";
    runTVDReconstructionTest();

    std::cout << "================================\n";
    std::cout << " Running CIRRUS Interpolator Unit Tests\n";
    std::cout << "================================\n\n";

    std::cout << "\n---- Linear Interpolator Test ----\n\n";
    runInterpolatorTest();
    std::cout << "\n---- Vector Linear Interpolator Test ----\n\n";
    runVectorInterpolatorTest();
    std::cout << "\n---- Bilinear Interpolator Test ----\n\n";
    runBilinearInterpolatorTest();

    std::cout << "\n---- PointValueQoI Test ----\n\n";
    runPointValueQoITest();

    std::cout << " \n----- All Interpolator Tests Passed ------\n\n";

    std::cout << "================================\n";
    std::cout << " Running CIRRUS Preconditioner Unit Tests\n";
    std::cout << "================================\n";

    std::cout << "\n---- SSOR Preconditioner Test ----\n\n";

    testSSORApplication();
    testSSORInvalidOmega();
    testSSORZeroDiagonal();
    testSSORSGSEquivalence();
    testSSORNonsymmetric();

    std::cout << "\n================================\n";
    std::cout << " Running Flux Limiter Unit Tests\n";
    std::cout << "================================\n\n";

    runFluxLimiterTest();

    std::cout << "\n==============================\n";
    std::cout << " COMPLETE\n";
    std::cout << "==============================\n";
    
    return 0;
}
