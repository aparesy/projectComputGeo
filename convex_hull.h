#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <random>
#include <cassert>
#include <vector>
#include <algorithm>
#include <tuple>
#include <set>

using namespace std;

using ld = long double; // For more precision

mt19937 rng(time(0)); //Random number generator of 32 bits. It is better than rand() which stops at 32767

// Simple macros to help
#define forn(i, n) for (int i = 0; i < n; i++) 
#define all(v) v.begin(), v.end()

// Constants

const int PRECISION=12;
#ifdef PERF
    const int NBPOINTS=3e4;
#else   
    const int NBPOINTS=100;
#endif
const int NB_ITER=4;
const int HLENGTH = 1200;
const int VLENGTH = 800;



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

    point operator/(ld z){
        return {x/z, y/z};
    }
};