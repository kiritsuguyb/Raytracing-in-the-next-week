#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"

using namespace std;
bvh_node random_scene(){
    hittable_list world;
	auto checher = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<lambertian>(checher);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
    
    for (int a = -11; a < 11; a++){
        for(int b=-11;b<11;b++){
            auto choose_mat=random_double();
            point3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2=center+vec3(0,random_double(0,0.5),0);
                    world.add(make_shared<moving_sphere>(center,center2,0.0,1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.3);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    //return world;
    return bvh_node(world,0,1);
}
hittable_list two_perlin_sphere() {
	hittable_list objects;

	auto pertex = make_shared<noise_texture>();
	objects.add(make_shared<sphere>(point3(0, 0, 0), 1000, make_shared<lambertian>(pertex)));
	//objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertex)));

	return objects;
}
color ray_color(const ray& r,const hittable& world,int depth){
    hit_record rec;

    if(depth<=0)return color(0,0,0);
    //我想知道这个射线传回的颜色
    if(world.hit(r,0.001,infinity,rec)){//嘿，我打到某个物体了，把他的信息记录到rec里
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r,rec,attenuation,scattered))//rec现在知道这个物体是谁了
        //麻烦rec告诉我这个物体的材质
        //这个物体的材质能告诉我会不会继续发射射线
        //会的话当前材质的颜色要和下一条射线传回来的颜色做一个反射率衰减
        //这样就能够得到这条射线的颜色了
            return attenuation*ray_color(scattered,world,depth-1);
        //如果材质说好吧没有下一次散射了，那也没关系，就跟上面的人说我再也没有了
        return color(0,0,0);
        // point3 target = rec.p + rec.normal + random_unit_vector();
        // return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }
    //如果运行到这里，看来这条射线什么东西也没打到，只能直接在背景里取一个颜色了
    //拿到这个颜色之后就带着这个颜色回到上一层，显然这个颜色会被上一层材质做一次衰减
    vec3 unit_direction=unit_vector(r.direction());
    auto t=0.5*(unit_direction.y()+1.0);
    return (1.0-t)*color(1.0,1.0,1.0)+t*color(0.5,0.7,1.0);
}
int main(){
	cerr << "----------" << setw(20) << "Program Start." << "----------" << endl << endl;
	clock_t start_time,temp_time;
	start_time = clock();

	ofstream fs;
	fs.open("triangle.ppm");
	fs.clear();

    const auto aspect_ratio=16.0/9.0;
    const int image_width=1000;
    const int image_height=static_cast<int>(image_width/aspect_ratio);
    const int samples_per_pixel=200;
    const int max_depth=10;

    fs<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";


    auto R=cos(pi/4);
	cerr.setf(ios::left);
    cerr<<"----------"<<setw(20)<<"Assembling Geometry." << "----------"<<endl << endl;

	auto world = two_perlin_sphere();
    
    temp_time=clock();
    cerr << endl <<"**********" << setw(20) << "Geometry Done."<<"**********"<<endl;
	cerr << "Time cost:" << double(clock() - temp_time) / CLOCKS_PER_SEC << "s" << endl << endl;
    

    point3 lookfrom(0,0,-2000);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus=2000;
    auto aperture=0.0;
    
    camera cam(lookfrom,lookat,vup,60,aspect_ratio,aperture,dist_to_focus,0.0,1.0);

	cerr << "----------"<< setw(20) << "Start raytracing." << "----------" << endl << endl;

    for(int j=image_height-1;j>=0;j--){
		cerr << "\rlines remaining:" << j << " " << flush;
		system("cls");
		cerr << setw(20) << "lines remaining:" << j << '\n'
			<< setw(20) << "Pixel count:" << pixel_count << '\n'
			<< setw(20) << "Ray count:" << ray_count << '\n'
			<< setw(20) << "Hitsearch count:" << hit_search_count << '\n'
			<< setw(20) << "Time cost:" << double(clock()- temp_time)/CLOCKS_PER_SEC <<"s"<< '\n'
			<< setw(20) << ' ' << flush;
        for(int i=0;i<image_width;i++){
			pixel_count++;
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; s++){
                auto u=double(i+random_double())/(image_width-1);
                auto v=double(j+random_double())/(image_height-1);
                ray r=cam.get_ray(u,v);
                pixel_color+=ray_color(r,world,max_depth);
            }
            write_color(fs,pixel_color,samples_per_pixel);
        }
    }
	cerr<< setw(20) << "Pixel count:" << pixel_count << '\n'
		<< setw(20) << "Ray count:" << ray_count << '\n'
		<< setw(20) << "Hitsearch count:" << hit_search_count << '\n'
		<< setw(20) << ' ' << endl;
	cerr << endl << "**********" << setw(20) << "Rartracing Done." << "**********" << endl;
    cerr<<"Time cost:"<<double(clock() -temp_time)/CLOCKS_PER_SEC<<"s"<<endl << endl;

	cerr << endl << "**********" << setw(20) << "Program Done." << "**********" << endl;
    cerr<< "Time cost:" << double(clock() -start_time)/CLOCKS_PER_SEC<<"s"<<endl << endl;

	fs.close();
}