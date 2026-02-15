#include <convex_hull.h>

// Structures for later use


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

class Algorithm2 {
private:
    std::vector<point> data; // Sorted based on the x coordinate.
public:
    std::vector<point> applyAlgorithm(std::vector<point> points) {
        if (points.empty())
            return std::vector<point>();
        if (points.size() <= 2)
            return points;
        point p_m{ getMedian(points) };
        std::vector<point> shuffled{ shuffle(points) };
        point p1{ points[0] }, p2{ *std::prev(points.end()) };
        ld lowestY = findIntersectionY(p_m.x, p1, p2);
        for (int k = 0; k < points.size(); ++k) {
            point p = points[k];
            if (findIntersectionY(p.x, p1, p2) < p.y) {
                p1 = p;
                p2 = p1 == points[0] ? points[1] : points[0];
                lowestY = findIntersectionY(p_m.x, p1, p2);
                for (int i = 0; i < k; ++i) {
                    std::tuple cur = std::tuple(findIntersectionY(p_m.x, p1, p), p1, p);
                    tie(lowestY, p1, p2) = std::min(std::tuple(lowestY, p1, p2), cur, 
                        [](std::tuple<ld, point, point> q1, std::tuple<ld, point, point> q2) { 
                            return std::get<0>(q1) < std::get<0>(q2); });
                }
            }
        }
        std::vector<point> res, lo, hi;
        for (auto p : points) {
            if (p.x < p1.x)
                lo.push_back(p);
            else if (p.x > p2.x)
                hi.push_back(p);
        }
        std::reverse(hi.begin(), hi.end());
        for (auto p : applyAlgorithm(lo))
            res.push_back(p);
        res.push_back(p1);
        res.push_back(p2);
        for (auto p : applyAlgorithm(hi))
            res.push_back(p);
        
        return res;
    }

    long double findIntersectionY(ld x_m, point p1, point p2) {
        return (p2.y - p1.y) / (p2.x - p1.x) * (x_m - p1.x) + p1.y;
    }

    point getMedian(std::vector<point> points) {
        return points[points.size() / 2];
    }

    std::vector<point> shuffle(std::vector<point> points) {
        std::vector<point> shuffled{ points };
        std::shuffle(points.begin(), points.end(), rng);
        
        return shuffled;
    }

    std::vector<point> startAlgorithm() {
        return applyAlgorithm(data);
    }

    Algorithm2(std::vector<point> points): data{ points } {
        sort(data.begin(), data.end());
    }
};

int main() {
    auto points=genC(NBPOINTS);
    freopen("output.txt", "w", stdout);
    // auto h=convex_hull_sweeping(points);
    Algorithm2 algo(points);
    auto h = algo.startAlgorithm();
    for (auto pt : h) cout<<pt.x<<" "<<pt.y<<"\n";
}
