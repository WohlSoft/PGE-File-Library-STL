#include <string>

int main()
{
    long x1 = std::stol("42");
    unsigned long long x2 = std::stoull("42");
    float x3 = std::stof("42.0");
    double x4 = std::stod("42.0");

    return 0;
}
