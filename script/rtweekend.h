#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include<cmath>
#include<cstdlib>
#include<limits>
#include<memory>

//Using
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//constants
const double infinity=std::numeric_limits<double>::infinity();
const double pi=3.1415926535897932385;

//Utility Functions

inline double degrees_to_radians(double degrees){
    return degrees*pi/180;
}

inline double random_double(){
    return rand()/(RAND_MAX+1.0);//RAND_MAX=2^8-1,是一个2字节数
}

inline double random_double(double min,double max){
    return min+(max-min)*random_double();
}

inline double clamp(double x,double min,double max){
    if(x<min)return min;
    if(x>max)return max;
    return x;
}

inline int clamp(int x,int min,int max){
    if(x<min)return min;
    if(x>max)return max;
    return x;
}

//common Headers

#include "ray.h"
#include "vec3.h"

#endif