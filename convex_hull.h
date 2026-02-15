#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <random>
#include <cassert>
#include <vector>
#include <algorithm>
#include <tuple>

using namespace std;

using ld = long double; // For more precision

mt19937 rng(time(0)); //Random number generator of 32 bits. It is better than rand() which stops at 32767

// Simple macros to help
#define forn(i, n) for (int i = 0; i < n; i++) 
#define all(v) v.begin(), v.end()

// Constants

const int PRECISION=12;
const int NBPOINTS=500;

// Structs 

struct point{
    ld x,y;

    point(ld x, ld y): x{ x }, y{ y } {}

    point operator+(point other) {
        return point(x + other.x, y + other.y);
    }

    point operator-(point other) {
        return point(x - other.x, y - other.y);
    }

    bool operator<(point other) {
        return x < other.x;
    }

    bool operator==(point other) {
        return x == other.x && y == other.y;
    }
};