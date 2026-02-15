#include "convex_hull.h"



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

template <typename T>
class MedianList {
private:
    using list_t = std::vector<T>;

    // Partitions 'lst' into two lists according to the 'pivot'.
    static std::pair<list_t, list_t> partition(list_t lst, T pivot) {
        list_t p1, p2;
        for (auto x : lst)
            (x < pivot ? p1 : p2).push_back(x);
        
        return std::pair(p1, p2);
    }

    static T quickSelect(list_t lst, int k) {
        if (lst.size() <= 5) {
            sort(lst.begin(), lst.end());
            return lst[k - 1];
        }
        list_t p1, p2;
        T pivot = findMedianOfMedians(lst);
        tie(p1, p2) = partition(lst, pivot);
        if ((int)p1.size() >= k)
            return quickSelect(p1, k);
        else if (k > (int)(lst.size() - p2.size()))
            return quickSelect(p2, k - (lst.size() - p2.size()));

        return pivot;
    }

    static T findMedianNaive(list_t lst) {
        list_t tmp{ lst };
        std::sort(tmp.begin(), tmp.end());

        return tmp[tmp.size() / 2];
    }
    
    static T findMedianOfMedians(list_t lst) {
        if ((int)lst.size() <= 5)
            return findMedianNaive(lst);
        list_t nxt;
        for (int i = 0; i < (int)lst.size(); i += 5) {
            list_t cur;
            for (int j = i; j < min(i + 5, static_cast<int>(lst.size())); ++j)
                cur.push_back(lst[j]);
            nxt.push_back(findMedianNaive(cur));
        }
        
        return findMedianOfMedians(nxt);
    }

public:
    // Note: this function is made to return a value below the median if the
    // list has an odd number of elements. This is intentional and is used in
    // the convex hull algorithm.
    static T findMedian(list_t lst) {
        T m1{ quickSelect(lst, lst.size() / 2) };
        T m2{ quickSelect(lst, lst.size() / 2 + 1) };
        return (m1 + m2) / 2;
    }
};

// Encapsulates the functions related to the divide-and-conquer approach to
// finding the convex hull.
class Algorithm2 {
private:
    std::vector<point> data;
public:
    // Subroutine for the main algorithm. Is used recursively.
    std::vector<point> applyAlgorithm(std::vector<point> points) {
        if (points.size() <= 2)
            return points;
        point p_m{ MedianList<point>::findMedian(points) };
        std::vector<point> shuffled{ shuffle(points) };

        point mini{ *std::min_element(points.begin(), points.end()) };
        point maxi{ *std::max_element(points.begin(), points.end()) };

        // Base points.
        point p1{ mini }, p2{ maxi };

        ld lowestY = findIntersectionY(p_m.x, p1, p2);
        for (int k = 0; k < (int)points.size(); ++k) {
            point p = points[k];
            if (p == p1 || p == p2)
                continue;
            if (findIntersectionY(p.x, p1, p2) >= p.y)
                continue;
            p1 = p;
            p2 = p1.x > p_m.x ? mini : maxi;
            lowestY = -INFINITY;
            for (int i = 0; i < k; ++i) {
                if ((p1.x - p_m.x) * (points[i].x - p_m.x) > 0.)
                    continue;
                std::tuple cur = std::tuple(findIntersectionY(p_m.x, p1, points[i]), p1, points[i]);
                tie(lowestY, p1, p2) = std::max(std::tuple(lowestY, p1, p2), cur, [](std::tuple<ld, point, point> q1, std::tuple<ld, point, point> q2) { return std::get<0>(q1) < std::get<0>(q2); });
            }
            if (p1.x > p2.x)
                swap(p1, p2);
        }

        // Create recursive cases.
        std::vector<point> res, lo, hi;
        for (auto p : points) {
            if (p.x <= p1.x)
                lo.push_back(p);
            else if (p.x >= p2.x)
                hi.push_back(p);
        }
        std::vector<point> loPoints = applyAlgorithm(lo);
        std::vector<point> hiPoints = applyAlgorithm(hi);

        // Reconstitute the final answer.
        for (auto p : loPoints)
            res.push_back(p);
        res.push_back(p1);
        res.push_back(p2);
        for (auto p : hiPoints)
            res.push_back(p);
        
        return res;
    }

    // Assumes 'p1' != 'p2'. Finds the y-coordinate of the point of (p1, p2)
    // whose x-coordinate is 'x_m'.
    long double findIntersectionY(ld x_m, point p1, point p2) {
        return (p2.y - p1.y) / (p2.x - p1.x) * (x_m - p1.x) + p1.y;
    }

    std::vector<point> shuffle(std::vector<point> points) {
        std::vector<point> shuffled{ points };
        std::shuffle(points.begin(), points.end(), rng);
        
        return shuffled;
    }

    std::vector<point> startAlgorithm() {
        std::vector<point> lst1{ applyAlgorithm(data) };

        // Points flipped around the y = 0 axis in order to find the lower hull.
        std::vector<point> negData{ data };
        for (auto& x : negData)
            x.y = -x.y;
        
        std::vector<point> lst2 = applyAlgorithm(negData);
        for (auto& x : lst2)
            x.y = -x.y;
        
        // Delete redundance with the upper hull.
        lst2.erase(prev(lst2.end()));
        lst2.erase(lst2.begin());

        reverse(lst2.begin(), lst2.end());
        std::vector<point> res{ lst1 };
        for (auto x : lst2)
            res.push_back(x);
        auto it { std::unique(res.begin(), res.end()) };
        res.erase(it, res.end());
    
        return res;
    }

    Algorithm2(std::vector<point> points): data{ points } {}
};

int main() {
    // In/out optimization
    cin.tie(0);

    cout<<"Results will be displayed in resultsX.txt\n";
    vector<point> data[4];

    freopen("results.txt", "w", stdout);
    #ifdef PERF
        cerr<<"All times are averaged over " << NB_ITER << " independant runs\n";
        vector<clock_t> times_sweep(4);
        vector<clock_t> times_dnc(4);
        vector<point> res[4];
        forn(i,4){
            forn(j,NB_ITER){
                switch (i)
                {
                case 0:
                    data[0] = genA(NBPOINTS);
                    break;
                case 1:
                    data[1] = genB(NBPOINTS);
                    break;
                case 2:
                    data[2] = genC(NBPOINTS);
                    break;
                case 3:
                    data[3] = genD(NBPOINTS);
                    break;
                }
                times_sweep[i]-=clock();
                res[i]=convex_hull_sweeping(data[i]);
                times_sweep[i]+=clock();
                times_dnc[i]-=clock();
                Algorithm2 tmp(data[i]);
                res[i]=tmp.startAlgorithm();
                times_dnc[i]+=clock();
            }
            times_sweep[i]/=((ld)(NB_ITER));
            times_dnc[i]/=((ld)(NB_ITER));
            cerr<<"Sweep for dataset "<<(char)('A'+i)<<" took "<<(times_sweep[i])*1.0/CLOCKS_PER_SEC<<" seconds on average\n";
            cerr<<"Dnc for dataset "<<(char)('A'+i)<<" took "<<(times_dnc[i])*1.0/CLOCKS_PER_SEC<<" seconds on average\n";
        }
        freopen("resultsA.txt", "w", stdout);
        for (auto pt : res[0]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsB.txt", "w", stdout);
        for (auto pt : res[1]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsC.txt", "w", stdout);
        for (auto pt : res[2]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsD.txt", "w", stdout);
        for (auto pt : res[3]) cout<<pt.x<<" "<<pt.y<<"\n";
    #endif

    #ifdef SAMPLE
        data[0]=genA(NBPOINTS);
        data[1]=genB(NBPOINTS);
        data[2]=genC(NBPOINTS);
        data[3]=genD(NBPOINTS);
        freopen("results.txt", "w", stdout);
        cout<<"Successfully generated datasets after ";
        cout<<clock()*1.0/CLOCKS_PER_SEC;
        cout<<" seconds\n";
        vector<point> res[4];
        vector<point> res2[4];
        forn(i,4){
            res[i]=convex_hull_sweeping(data[i]);
            Algorithm2 tmp(data[i]);
            res2[i]=tmp.startAlgorithm();
            if (res[i].size()+1==res2[i].size()) res2[i].pop_back();
        }
        freopen("resultsA.txt", "w", stdout);
        for (auto pt : res2[0]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsB.txt", "w", stdout);
        for (auto pt : res2[1]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsC.txt", "w", stdout);
        for (auto pt : res2[2]) cout<<pt.x<<" "<<pt.y<<"\n";
        freopen("resultsD.txt", "w", stdout);
        for (auto pt : res2[3]) cout<<pt.x<<" "<<pt.y<<"\n";

        freopen("results.txt", "w", stdout);
        cout<<"Do both algorithms return the same result ? \n";
        forn(i,4){
            cout<<"Running on dataset "<<(char)('A'+i)<<" : ";
            set<pair<ld,ld>> st;
            bool ok=true;
            if (res[i].size() != res2[i].size()){
                cout<<"The sizes of hulls returned are different ("<<res[i].size()<<" vs "<<res2[i].size()<<")\n";
                continue;
            }
            for (auto pt : res[i]){
                st.insert({pt.x, pt.y});
            }
            for (auto [a,b] : res2[i]){
                if (st.find({a,b})==st.end()){
                    cout<<fixed<<setprecision(PRECISION)<<"Couldn't find point ("<<a<<","<<b<<") in the sweeping hull\n";
                    ok=false;
                }
            }
            if (ok) cout<<"OK, hulls are the same\n";
            else cout<<"Hulls are different\n";
        }
    #endif
}
