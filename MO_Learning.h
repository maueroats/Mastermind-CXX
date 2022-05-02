//
// Created by docmo on 4/29/22.
//

#ifndef DEV_CXX_MO_LEARNING_H
#define DEV_CXX_MO_LEARNING_H
#include <boost/unordered_set.hpp>

namespace MO_Learning {

    typedef std::pair<int, int> ii;
    typedef std::vector<int> code;

    class Solver {
    public:
        const int n;
        const int k;
        boost::unordered_set<int> possible;
        bool solved = false;
        int solution;
        int all;

        Solver(int num_spots, int num_colors);
        ii score(code secret, code guess);
        ii score_num(int secret, int guess);
        std::vector<int> static number_base(int spots, int base, int n);
        void register_feedback(int guess, int exact, int approximate);
        int evaluate_one_best(int guess);
        int evaluate_one_best_orig(int guess); // to not throw it away yet
        int generate_guess();
        int kt(int num, int digit_start = 1);
        int tk(int num, int digit_start = 1);
    }; // Solver

    int play(int num_spots, int num_colors, int secret, bool convert_secret = false, bool verbose = false, int guess_limit = 20);
};
#endif //DEV_CXX_MO_LEARNING_H