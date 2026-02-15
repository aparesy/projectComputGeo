#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <random>
#include <cassert>
#include <vector>
#include <algorithm>

#include <SFML/Graphics.hpp>

using namespace std;

using ld = long double; // For more precision

mt19937 rng(time(0)); //Random number generator of 32 bits. It is better than rand() which stops at 32767

// Simple macros to help
#define forn(i, n) for (int i = 0; i < n; i++) 
#define all(v) v.begin(), v.end()

// Constants

const int PRECISION=12;
const int NBPOINTS=50;
const int HLENGTH=1600;
const int VLENGTH=1000;
// Structures for later use

struct point{
    ld x,y;
};

/* Part 2 : datasets*/

// Functions to generate point datasets. After generation, points can be found in the "datasetX.txt" files

ld random_double(){
    // Generates a random ld by taking rng() (a 32 bits random integer generator), and dividing it by its maximum value (INT_MAX)
    return ((ld)(rng())/((ld)(UINT32_MAX)));
}

vector<point> genA(int n){
    // Generates n-4 points randomly in the [0,1]^2 square, and adds the corners. Applies a random rotation and a random shuffling
    // The resulting points can be outside of [0,1]^2 but it doesn't matter. The center of rotation is chosen randomly in [0,1]^2
    // If n<4, this function returns an error 
    assert(n>=4 && "There are less than 4 points given to genA !");

    vector<point> points; 
    point rotation_center={random_double(), random_double()};
    ld rotation_angle=random_double()*2.*M_PI;

    // Define a lambda-function for the rotation (even though there is probably one somewhere in the stl)
    auto rotate = [&](point pt){
        point centerToPt={pt.x-rotation_center.x, pt.y-rotation_center.y}; // Take the vector from center to target point
        ld c=cos(rotation_angle);
        ld s=sin(rotation_angle);
        point rotated={centerToPt.x*c-centerToPt.y*s, centerToPt.x*s+centerToPt.y*c}; //Apply the well-known rotation matrix Rtheta
        rotated.x+=rotation_center.x; rotated.y+=rotation_center.y;
        return rotated;
    };
    forn(i,n-4){
        point cur_point={random_double(), random_double()};
        points.push_back(rotate(cur_point));
    }
    vector<point> corners;
    corners.push_back({0.,0.});
    corners.push_back({1.,0.});
    corners.push_back({0.,1.});
    corners.push_back({1.,1.});
    for (auto corner : corners){
        points.push_back(rotate(corner));
    }

    shuffle(all(points), rng); // STL function to shuffle (I could do it myself by just n random positions)

    freopen("datasetA.txt","w", stdout);
    for (auto pt : points) cout<<fixed<<setprecision(PRECISION)<<pt.x<<" "<<pt.y<<"\n";
    return points;
}

vector<point> genB(int n){
    freopen("datasetB.txt","w", stdout);
    vector<point> points;
    forn(i,n){
        ld x=random_double();
        ld y=random_double();
        points.push_back({x,y});
        cout<<fixed<<setprecision(PRECISION)<<x<<" "<<y<<"\n"; 
    }
    return points;
}

vector<point> genC(int n){
    vector<point> points;
    return points;
}

vector<point> genD(int n){
    vector<point> points;
    return points;
}

/* Part 3 : Sweeping */

ld cross(point a, point b){return a.x*b.y-a.y*b.x;}

bool orient(point a, point b, point c){
    // Returns 1 if clockwise and 0 if counter-clockwise.
    // Stems from a x b = ||a|| ||b|| sin(theta) where x is cross product
    point a_to_b={b.x-a.x, b.y-a.y};
    point a_to_c={c.x-a.x, c.y-a.y};
    return (cross(a_to_b, a_to_c)<=0); //Negative means theta is negative means angle is clockwise 
}

vector<point> convex_hull_sweeping(vector<point>& points){
    sort(all(points), [](const point a, const point b){
        if (a.x==b.x) return a.y<b.y;
        return a.x<b.x;
    }); // Sort points by increasing x using a lambda function
    int n=points.size();
    vector<point> hull_up;
    vector<point> hull_down;
    // Increasing x pass
    forn(i,n){
        if(hull_up.size()<=1){ // If there are 0 or 1 points in the hull
            hull_up.push_back(points[i]);
        }
        else{
            // Compare orientation with the two previous points
            point a=hull_up[hull_up.size()-2];
            point b=hull_up[hull_up.size()-1];
            point c=points[i];
            if (orient(a,b,c)){
                // If clockwise, meaning c is to the right side of b
                hull_up.push_back(c);
            }
            else{
                while(!orient(a,b,c) && (int)(hull_up.size())>=3){
                    hull_up.pop_back(); // Remove the last point of the hull
                    b=hull_up[hull_up.size()-1];
                    a=hull_up[hull_up.size()-2];
                }
                if (!orient(a,b,c)){
                    hull_up.pop_back();
                }
                hull_up.push_back(c);
            }
        }
    }
    // Decreasing x pass. If everything went well the rightmost point and leftmost point are in the hull so it's no use to consider them
    for(int i=n-1;i>=0;i--){ 
        if(hull_down.size()<=1){
            hull_down.push_back(points[i]);
        }
        else{
            point a=hull_down[hull_down.size()-2];
            point b=hull_down[hull_down.size()-1];
            point c=points[i];
            if (orient(a,b,c)){
                hull_down.push_back(c);
            }
            else{
                while(!orient(a,b,c) && (int)(hull_down.size())>=3){
                    hull_down.pop_back(); 
                    b=hull_down[hull_down.size()-1];
                    a=hull_down[hull_down.size()-2];
                }
                if (!orient(a,b,c)){
                    hull_down.pop_back();
                }
                hull_down.push_back(c);
            }
        }
    }
    for (int j=1;j<(int)(hull_down.size())-1;j++) hull_up.push_back(hull_down[j]);
    return hull_up;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(HLENGTH, VLENGTH), "Live visualizer");
    sf::RectangleShape background(sf::Vector2f(HLENGTH, VLENGTH));
    background.setFillColor(sf::Color::White);
    while(window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(background);
        window.display();
    }
    auto points=genB(NBPOINTS);
    freopen("output.txt", "w", stdout);
    auto h=convex_hull_sweeping(points);
    for (auto pt : h) cout<<pt.x<<" "<<pt.y<<"\n";
}
