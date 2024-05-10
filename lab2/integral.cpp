#include "arg-parser.h"
#include <algorithm>


int main(int argc, char** argv) {
    ArgParser parser(argc, argv, "n:p:");

    int num_proces = parser.get_arg<int>('n');
    double precision = parser.get_arg<double>('p');

    auto result = compute_integral(function, num_proces, precision);

    std::cout << "Result:\n\t Value = " << result.result << "\n\tTime = " << result.time << "\n";

    return 0;
}
