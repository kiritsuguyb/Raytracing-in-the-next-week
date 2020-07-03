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

//Debug variales
long long hit_search_count = 0;
long long ray_count = 0;
int pixel_count = 0;


//Utility Functions

inline double degrees_to_radians(double degrees){
    return degrees*pi/180;
}

inline double random_double(){
    return rand()/(RAND_MAX+1.0)+0.0000001;//RAND_MAX=2^8-1,是一个2字节数
}

inline double random_double(double min,double max){
    return min+(max-min)*random_double();
}

inline int random_int(){
    return (int)random_double();//RAND_MAX=2^8-1,是一个2字节数
}

inline int random_int(int min,int max){
    return (int)random_double(min,max);
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