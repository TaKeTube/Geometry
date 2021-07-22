#ifndef HE_INTEGRAL_H
#define HE_INTEGRAL_H

#include <cmath>
#include <vector>
#include <functional>

namespace HexEval
{
    double integrateRectangle(double a, double b, std::function<double(double)> f, int n);
    double integrateTrapeze(double a, double b, std::function<double(double)> f, int n);
    double integrateSimpson(double a, double b, std::function<double(double)> f, int n);
    double newtonCotes2(double a, double b, double n, std::function<double(double)> f);
    double newtonCotes3(double a, double b, double n, std::function<double(double)> f);
    double gauss2(double a, double b, double n, std::function<double(double)> f);
    double gauss3(double a, double b, double n, std::function<double(double)> f);
    double integrateBoole(double StartPoint, double EndPoint, int n, std::function<double(double)> f);
}

#endif