#include <stdio.h>

typedef double (*MathFunc) (double);

typedef struct {
    double upper_limit, lower_limit;
    double dx;
    MathFunc func;
} Integral;

Integral new_integrar(double lower, double upper, double dx, MathFunc func) {
    return (Integral) {
        upper, lower, dx, func
    };
}

void run(Integral integ) {
    for (double i = integ.lower_limit; i < integ.upper_limit; i += integ.dx) 
        printf("res: %g\n", integ.func(i));
}

double porabola(double x) {
    return x * x;
}

