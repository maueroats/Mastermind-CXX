//
// Created by docmo on 4/29/22.
//

#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <fmt/core.h>
#include <boost/unordered_map.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/accumulators/statistics/stats.hpp>
//#include <random>
#include <boost/test/unit_test.hpp>
#include "Xoshiro256.h"
#include "MO_Learning.h"

using namespace std;
using namespace boost::accumulators;
namespace MO_Learning {

    const bool debug = true;
    Xoshiro256 rng = Xoshiro256::default_rng();

    Solver::Solver(int num_spots, int num_colors) : n(num_spots), k(num_colors),
                                                    all(pow(num_colors, num_spots)),
                                                    solution(-1) {
        possible.reserve(all);
        for (int kk = 0; kk < all; kk++) {
            possible.insert(kk);
        }
    }

    BOOST_AUTO_TEST_CASE(test_constructor) {
        Solver s(4, 6);
        BOOST_CHECK_EQUAL(pow(6, 4), s.all);
        BOOST_CHECK_EQUAL(pow(6, 4), s.possible.size());
        BOOST_TEST(s.possible.contains(0));
        BOOST_TEST(s.possible.contains(1));
        BOOST_TEST(s.possible.contains(1235));
    }

    ii Solver::score(code secret, code guess) {
        int exact = 0;
        std::vector<int> cs = std::vector<int>(k, 0);
        std::vector<int> cg = std::vector<int>(k, 0);
        for (auto const &[sx, gx]: boost::combine(secret, guess)) {
            if (sx == gx) {
                exact += 1;
            }
            cs[sx] += 1;
            cg[gx] += 1;
        }

        int inexact = 0;
        for (auto const &[ys, yg]: boost::combine(cs, cg)) {
            inexact += min(ys, yg);
        }

        return {exact, inexact - exact};
    }

    std::vector<int> Solver::number_base(int spots, int base, int num) {
        std::vector<int> answer;
        while (spots > 0) {
            auto digit = num % base;
            answer.push_back(digit);
            num /= base;
            spots--;
        }
        return answer;
    }

    BOOST_AUTO_TEST_CASE(test_number_base) {
        std::vector<int> correct = {3, 1, 2, 2};
        int original = 3 + 1 * 6 + 2 * 6 * 6 + 2 * 6 * 6 * 6;
        Solver s(4, 6);
        BOOST_TEST(correct == Solver::number_base(4, 6, original),
                   "2213 base 6 is 513");
    }

    ii Solver::score_num(int secret, int guess) {
        return score(number_base(n, k, secret),
                     number_base(n, k, guess));
    }

    BOOST_AUTO_TEST_CASE(test_score_num) {
        int secret = ((((5 * 6 + 1) * 6) + 2) * 6 + 2); // 5122
        int guess = (((1 * 6 + 5) * 6 + 2) * 6 + 4); // 1524
        ii correct(1, 2);
        Solver s(4, 6);
        bool ok = (correct == s.score_num(secret, guess));
        BOOST_TEST(ok, "score for 5122 vs 1524 is (1,2)");
    }

    void Solver::register_feedback(int guess, int exact, int approximate) {
        boost::unordered_set<int> new_possible;
        std::pair<int, int> response(exact, approximate);
        for (auto x: possible) {
            if (response == score_num(x, guess)) {
                new_possible.insert(x);
            }
        }
        possible = new_possible;
        if (possible.size() == 1) {
            solved = true;
            solution = *possible.begin();
        }
        if (possible.empty()) {
            throw runtime_error("impossible: so solutions in register_feedback");
        }
    }

    BOOST_AUTO_TEST_CASE(test_register_feedback) {
        Solver s(4, 6);
        // int secret = ((((5 * 6 + 1) * 6) + 2) * 6 + 2); // 5122
        int guess = (((1 * 6 + 5) * 6 + 2) * 6 + 4); // 1524
        int exact = 1;
        int approximate = 2;
        BOOST_CHECK_EQUAL(pow(6, 4), s.possible.size());
        s.register_feedback(guess, exact, approximate);
        BOOST_CHECK_EQUAL(132, s.possible.size()); // confirmed by Python program
    }

    int Solver::evaluate_one_best_orig(int guess) {
        // purpose: return the number of possibilities remaining in the worst case scenario for this guess
        // OLD CODE:
        boost::unordered_map<ii, int> counts;
        for (auto secret: possible) {
            auto val = score_num(secret, guess);
            auto old_count = counts.find(val);
            if (old_count == counts.end()) {
                counts[val] = 1;
            } else {
                counts[val] = 1 + old_count->second;
            }
        }
        int best = 0;
        for (auto &[response, val]: counts) {
            best = max(best, val);
        }
        return best;
    }

    int Solver::evaluate_one_best(int guess) {
        // purpose: return the number of possibilities remaining in the worst case scenario for this guess
        std::vector<int> count_v = std::vector<int>(n * n, 0);
        for (auto const &secret: possible) {
            auto val = score_num(secret, guess);
            count_v[n * val.first + val.second] += 1;
        }
        return *std::max_element(count_v.begin(), count_v.end());

    }

    BOOST_AUTO_TEST_CASE(test_evaluate_one_best) {
        Solver s(4, 6);
        s.register_feedback(153, 1, 2);
        BOOST_CHECK_EQUAL(132, s.possible.size());
        BOOST_CHECK_EQUAL(64, s.evaluate_one_best(688)); // guess = 4215
        BOOST_CHECK_EQUAL(40, s.evaluate_one_best(46)); // guess = 1225
    }

    int Solver::generate_guess() {
        int best_worst = all;
        std::vector<int> winners;
        for (auto g: boost::counting_range(0, all)) {
            auto resp = evaluate_one_best(g);
            if (resp < best_worst) {
                winners.clear();
                best_worst = resp;
            }
            if (resp <= best_worst) {
                winners.push_back(g);
            }
        }
        if (winners.empty()) {
            throw runtime_error("generate_guess: no possibilities were found");
        }
        std::vector<int> winners_best;
        for (auto g: winners) {
            if (possible.contains(g)) {
                winners_best.push_back(g);
            }
        }
        int result;
        // selection is random in Python version
        if (!winners_best.empty()) {
            result = winners_best[rng.next() % winners_best.size()];
        } else {
            cerr << "generate_guess: warning - best move is not possible (" << winners.size() << " possibilities)" << endl;
            result = winners[rng.next() % winners.size()];
        }
        return result;
    }

    BOOST_AUTO_TEST_CASE(test_generate_guess_1, *boost::unit_test::enabled()) {
        // sequence of guesses with correct = 140
        Solver s(4, 6);
        s.register_feedback(153, 1, 1);
        BOOST_CHECK_EQUAL(252, s.possible.size());
        int stage_one_guess = s.generate_guess();
        BOOST_CHECK_EQUAL(8, stage_one_guess);
        s.register_feedback(stage_one_guess, 2, 0);
        BOOST_CHECK_EQUAL(28, s.possible.size());
        int stage_two_guess = s.generate_guess();
        BOOST_CHECK_EQUAL(731, stage_two_guess);
        s.register_feedback(stage_two_guess, 0, 3);
        BOOST_CHECK_EQUAL(2, s.possible.size());
        int stage_three_guess = s.generate_guess();
        BOOST_CHECK_EQUAL(140, stage_three_guess);
        s.register_feedback(140, 4, 0);
        BOOST_CHECK_EQUAL(1, s.possible.size());
    }

    int Solver::kt(int num, int digit_start) {
        int ans = 0;
        int b = 1;
        int digits = n;
        while (digits > 0) {
            ans += b * (num % k + digit_start);
            b *= 10;
            num /= k;
            digits -= 1;
        }
        return ans;
    }

    int Solver::tk(int num, int digit_start) {
        int ans = 0;
        int b = 1;
        while (num > 0) {
            ans += b * (num % 10 - digit_start);
            b *= k;
            num /= 10;
        }
        return ans;
    }

    BOOST_AUTO_TEST_CASE(test_kt_digits) {
        Solver s(4, 6);
        BOOST_CHECK_EQUAL(1525, s.kt(154));
    }

    BOOST_AUTO_TEST_CASE(test_tk_digits) {
        Solver s(4,6);
        BOOST_CHECK_EQUAL(154, s.tk(1525));
    }

    int play(int num_spots, int num_colors, int secret, bool convert_secret, bool verbose, int guess_limit) {
        int n = num_spots;
        int k = num_colors;
        auto knuth = Solver(n, k);
        bool finished = false;
        int count = 0;

        if (convert_secret) {
            secret = knuth.tk(secret);
        }

        if (verbose) {
            std::cout << "Guessing the secret code: " << knuth.kt(secret) << std::endl;
        }
        while (!finished && count < guess_limit) {
            count += 1;
            auto guess = knuth.generate_guess();
            auto [exact, approximate] = knuth.score_num(secret, guess);
            finished = (exact == n);
            knuth.register_feedback(guess, exact, approximate);

            if (verbose) {
                std::cout << fmt::format("{:03}\t{}\t{} {}\n",
                                         count, knuth.kt(guess), exact, approximate);
            }
        }
        if (verbose) {
            string s = "Guessed";
            if (!finished) { s = "Failed to guess"; }
            std::cout << s << " code in " << count << " tries" << std::endl;
        }
        return count;
    }
}
