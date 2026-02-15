#include "convex_hull.h"

#include <SFML/Graphics.hpp>



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
    // Using reject-accept algorithm in a circle
    freopen("datasetC.txt","w", stdout);
    vector<point> points;
    while((int) points.size()<n){
        ld x=random_double();
        ld y=random_double();
        double d = hypot(x-0.5, y-0.5);
        if (d <=0.5){
           // cout<<d<<"\n";
            points.push_back({x,y});
            cout<<fixed<<setprecision(PRECISION)<<x<<" "<<y<<"\n"; 
        }
    }
    return points;
}

vector<point> genD(int n){
    freopen("datasetD.txt","w", stdout);
    vector<point> points;
    forn(i,n){
        ld theta=2.*M_PI*random_double();
        ld x = 0.5*cos(theta)+0.5;
        ld y = 0.5*sin(theta)+0.5;
        cout<<fixed<<setprecision(PRECISION)<<x<<" "<<y<<"\n"; 
        points.push_back({x,y});
    }
    return points;
}

/* Part 3 : Sweeping */

ld cross(point a, point b){return a.x*b.y-a.y*b.x;}

void draw_points(sf::RenderWindow& window,
                 const vector<point>& all_points,
                 const vector<point>& hull,
                 const vector<point>& highlighted = {}) 
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    window.clear(sf::Color::White);

    // Draw all points in black
    for (auto& p : all_points) {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        dot.setPosition(p.x * HLENGTH, p.y * VLENGTH);
        window.draw(dot);
    }

    // Draw hull points in green
    for (auto& p : hull) {
        sf::CircleShape dot(6);
        dot.setFillColor(sf::Color::Green);
        dot.setPosition(p.x * HLENGTH, p.y * VLENGTH);
        window.draw(dot);
    }

    // Draw currently processed points in red
    for (auto& p : highlighted) {
        sf::CircleShape dot(8);
        dot.setFillColor(sf::Color::Red);
        dot.setPosition(p.x * HLENGTH, p.y * VLENGTH);
        window.draw(dot);
    }

    window.display();
    sf::sleep(sf::milliseconds(40)); // Slow animation
}


bool orient(point a, point b, point c,
            sf::RenderWindow& window,
            const vector<point>& all_points,
            const vector<point>& hull)
{
    draw_points(window, all_points, hull, {a, b, c});

    point a_to_b = {b.x - a.x, b.y - a.y};
    point a_to_c = {c.x - a.x, c.y - a.y};
    return (cross(a_to_b, a_to_c) <= 0);
}

vector<point> convex_hull_sweeping(vector<point>& points,
                                   sf::RenderWindow& window){
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
            draw_points(window, points, hull_up);
        }
        else{
            // Compare orientation with the two previous points
            point a=hull_up[hull_up.size()-2];
            point b=hull_up[hull_up.size()-1];
            point c=points[i];
            if (orient(a,b,c,window,points,hull_up)){
                // If clockwise, meaning c is to the right side of b
                hull_up.push_back(c);
                draw_points(window, points, hull_up);
            }
            else{
                while(!orient(a,b,c,window,points,hull_up) && (int)(hull_up.size())>=3){
                    hull_up.pop_back(); // Remove the last point of the hull
                    b=hull_up[hull_up.size()-1];
                    a=hull_up[hull_up.size()-2];
                    draw_points(window, points, hull_up);
                }
                if (!orient(a,b,c,window,points,hull_up)){
                    hull_up.pop_back();
                    draw_points(window, points, hull_up);
                }
                hull_up.push_back(c);
                draw_points(window, points, hull_up);
            }
        }
    }
    // Decreasing x pass. If everything went well the rightmost point and leftmost point are in the hull so it's no use to consider them
    for(int i=n-1;i>=0;i--){ 
        if(hull_down.size()<=1){
            hull_down.push_back(points[i]);
            draw_points(window, points, hull_down);
        }
        else{
            point a=hull_down[hull_down.size()-2];
            point b=hull_down[hull_down.size()-1];
            point c=points[i];
            if (orient(a,b,c,window,points,hull_down)){
                hull_down.push_back(c);
                draw_points(window, points, hull_down);
            }
            else{
                while(!orient(a,b,c,window,points,hull_down) && (int)(hull_down.size())>=3){
                    hull_down.pop_back(); 
                    b=hull_down[hull_down.size()-1];
                    a=hull_down[hull_down.size()-2];
                    draw_points(window, points, hull_down);
                }
                if (!orient(a,b,c,window,points,hull_down)){
                    hull_down.pop_back();
                    draw_points(window, points, hull_down);
                }
                hull_down.push_back(c);
                draw_points(window, points, hull_down);
            }
        }
    }
    vector<point> final_hull = hull_up;

    for (int j = 1; j < (int)hull_down.size() - 1; j++)
        final_hull.push_back(hull_down[j]);

    draw_points(window, points, final_hull);
    return final_hull;
}

int main() {

    auto points = genB(NBPOINTS);

    sf::RenderWindow window(sf::VideoMode(HLENGTH, VLENGTH), "Convex Hull Visualizer");

    auto hull = convex_hull_sweeping(points, window);

    // Keep window open after animation
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
