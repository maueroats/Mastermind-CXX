#include <iostream>
#include <random>
#include "MO_Learning.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

int main(int argc, char *argv[]) {
    int num_colors;
    int num_spots;
    int secret_code;
    bool convert_secret = false;
    bool verbose = true;
    options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce a help message")
            ("colors", value<int>(&num_colors)->default_value(6),
                    "the maximum number of colors permitted in the pattern")
            ("spots", value<int>(&num_spots)->default_value(4),
                    "the number of spots (pegs) in the pattern")
            ("secret", value<int>(&secret_code)->default_value(-1),
                    "the secret code for the computer to guess. digits 1 through colors. (-1 means computer picks randomly)")
            ("verbose", "produce verbose output while playing")
            ("quiet", "no output while playing")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << endl;
        return 1;
    }

    if (vm.count("verbose")) {
        verbose = true;
    }

    if (vm.count("quiet")) {
        verbose = false;
    }

    if (num_colors == -1) {
        std::cout << "Number of colors: ";
        std::flush(std::cout);
        std::cin >> num_colors;
    }
    if (num_spots == -1) {
        std::cout << "Number of spots: ";
        std::flush(std::cout);
        std::cin >> num_spots;
    }

    if (secret_code != -1) {
        convert_secret = true;
    } else {
        std::random_device generator;
        std::uniform_int_distribution<int> rng(0,-1 + pow(num_colors, num_spots));
        secret_code = rng(generator);
//    cout << "Secret code: " << secret_code << endl;
    }

    int count = MO_Learning::play(num_spots, num_colors, secret_code, convert_secret, verbose);
    cout << "Discovered code in " << count << " tries" << endl;
    return 0;
}
