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
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"

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

	auto pertex = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertex)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertex)));

	return objects;
}
hittable_list earth() {
	hittable_list objects;
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
	objects.add(globe);

	return objects;
}
hittable_list simple_light() {
	hittable_list objects;
	
	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
	objects.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
	
	return objects;
}
hittable_list cornell_box() {
	hittable_list objects;
	auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
	auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
	auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
	auto light = make_shared<diffuse_light>(make_shared<solid_color>(15, 15, 15));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);
	return objects;
}
hittable_list cornell_smoke() {
	hittable_list objects;

	auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
	auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
	auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
	auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));
	
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));

	objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<solid_color>(0, 0, 0)));
	objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<solid_color>(1, 1, 1)));

	return objects;
}
color ray_color(const ray& r,const color& background,const hittable& world,int depth){
    hit_record rec;

    if(depth<=0)return color(0,0,0);
    //我想知道这个射线传回的颜色
    if(world.hit(r,0.001,infinity,rec)){//嘿，我打到某个物体了，把他的信息记录到rec里
        ray scattered;
        color attenuation;
		color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))//rec现在知道这个物体是谁了
		//麻烦rec告诉我这个物体的材质
		//这个物体的材质能告诉我会不会继续发射射线
		//会的话当前材质的颜色要和下一条射线传回来的颜色做一个反射率衰减
		//这样就能够得到这条射线的颜色了
			return emitted +attenuation*ray_color(scattered, background,world, depth - 1);
        //如果材质说好吧没有下一次散射了，那也没关系，就跟上面的人说我再也没有了
        return emitted;
        // point3 target = rec.p + rec.normal + random_unit_vector();
        // return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }
	else
	{
		return background;
	}
}
int main(){


	cerr << "----------" << setw(20) << "Program Start." << "----------" << endl << endl;
	clock_t start_time,temp_time;
	start_time = clock();

	ofstream fs;
	fs.open("triangle.ppm");
	fs.clear();

    const auto aspect_ratio=16.0/16;
	const color background(0, 0, 0);
    const int image_width=200;
    const int image_height=static_cast<int>(image_width/aspect_ratio);
    const int samples_per_pixel=1000;
    const int max_depth=100;
	

    fs<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";


    auto R=cos(pi/4);
	cerr.setf(ios::left);
    cerr<<"----------"<<setw(20)<<"Assembling Geometry." << "----------"<<endl << endl;

	auto world = cornell_smoke();
    
    temp_time=clock();
    cerr << endl <<"**********" << setw(20) << "Geometry Done."<<"**********"<<endl;
	cerr << "Time cost:" << double(clock() - temp_time) / CLOCKS_PER_SEC << "s" << endl << endl;
    

	point3 lookfrom(278, 278, -800);
	point3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;
    
    camera cam(lookfrom,lookat,vup, vfov,aspect_ratio,aperture,dist_to_focus,0.0,1.0);

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
			/*if (i!=image_width/2||j!=image_height/2)
			{
				write_color(fs, color(0,0,0), samples_per_pixel);
				continue;
			}*/
			pixel_count++;
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; s++){
                auto u=double(i+random_double())/(image_width-1);
                auto v=double(j+random_double())/(image_height-1);
                ray r=cam.get_ray(u,v);
                pixel_color+=ray_color(r,background,world,max_depth);
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