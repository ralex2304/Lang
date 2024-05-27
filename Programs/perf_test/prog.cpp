#include <math.h>

const double EPS = 1e-6;

double fact(const double i) {

    if (i > 1 && fabs(i - 1) >= EPS)
        return fact(i - 1) * i;

    return 1;
}

int main() {

    double N = 1e7;

    while (fabs(N) >= EPS  && N-- > 0)
        fact(20);

    return 0;
}
