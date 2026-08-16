#include <iostream>

void runScalarFieldTest();
void runVectorFieldTest();
void runFieldRegistryTest();
void runInterpolatorTest();

int main()
{
    std::cout << "================================\n";
    std::cout << " Running CIRRUS Unit Tests\n";
    std::cout << "================================\n\n";

    runScalarFieldTest();
    runVectorFieldTest();
    runFieldRegistryTest();

    std::cout << "\n================================\n";
    std::cout << " All Field Tests Passed\n";
    std::cout << "================================\n";

    return 0;
}
