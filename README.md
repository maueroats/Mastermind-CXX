
# Mastermind: C++ Version

A program to play the game of mastermind, guessing a number pattern like "1225" or "1663"
when given clues indicating how many digits are exactly correct and how many are
in the pattern but not in the correct location.

## Usage

Typical usage:
`mastermind --spots 4 --colors 6 --secret 1256`.
The program has to know what the "secret" is, although it doesn't use that information except
in generating feedback for itself. Compiled with `-O2` it can manage 4 colors in 9 spots, 
or 6 colors in 5 spots.

## Strategy

This program uses Donald Knuth's 1977 strategy of picking the move that results
in the fewest remaining possibilities in the worst case scenario. This is a kind
of minimax strategy. It solves the traditional 4 spot, 6 color game in at most five 
moves.

## Reflection

I wrote this code based on a working but slow Python version. Obviously, the C++
code runs a lot faster than the Python code. 

* I learned how to write unit tests
in C++ using the Boost.Test framework. This was great because basically every 
function I wrote had some bug in it. Immediate testing helped me track those
down a lot faster.
* The `fmt` formatter provides formatting very similar to that of Python. 
Totally recommended.
* I encountered several C++ warts on
my journey. (Did you know an unordered set only gained the `contains` method 
in C++ 20?! Wow. I went straight to the Boost version when I found this.)

### Matching Randomness in Python and C++
The most interesting part of the project was actually trying to match
the randomness on the Python side with something on the C++ side. 

One of the steps I use is to randomly choose among all of the patterns that have the same 
worst-case scenario. Since my Python program worked, I wanted to validate (debug) the C++
program by getting the exact same results. This meant I needed a way of making the same 
random choice in C++ and Python.

At first, this seemed easy. Both generate random numbers based on the Mersenne Twister
("mt19937"). Notes on this:
* Seeding is different. That means Python and the C++ `<random>` do different things
with the numbers you provide in order to seed the generator.
* Use of the random numbers to produce a uniform distribution is done in different ways.
This means even if you synchronize the Mersenne Twisters, you will not get the same 
sequence of integers out.

Solution: I used the [Xoshiro256](https://prng.di.unimi.it/) random number generator 
for both C++ and Python. 

I translated the C++ version to Python myself. In retrospect, it would have been
much easier to search for an [existing Python library containing an implementation
of Xoshiro256](https://bashtage.github.io/randomgen/bit_generators/xoshiro256.html).
However, this way I learned something.

**Lesson Learned.** Numpy's `uint64` type is easy to accidentally promote to `float64`, ruining 
an attempt to duplicate the operations in the C++ code. Make sure that everything
is explicitly `np.uint64`. For example: `s[1]*np.uint64(5)` instead
of `s[1]*5`, and `s[1] << np.uint8(17)` instead of `s[1]<<17`. I don't think
the actual choice of which unsigned Numpy type matters; I used both `np.uint8` and 
`np.uint64` with no apparent difference. (All of the numbers I used fit in 8 bits.)
