#ifndef MATERIAL_H
#define MATERIAL_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "texture.h"


class material{
    public:
        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered
        )const=0;

		virtual color emitted(double u, double v, const point3& p)const {
			return color(0, 0, 0);
		}
};

double schlick(double cosine,double ref_idx){
    auto r0=(1-ref_idx)/(1+ref_idx);
    r0=r0*r0;
    return r0+(1-r0)*pow((1-cosine),5);
}
class lambertian: public material{
    public:
        lambertian(const shared_ptr<texture> a): albedo(a){}
		lambertian(const color a) : albedo(make_shared<solid_color>(a)) {}

        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered
        )const{
            vec3 scatter_direction=rec.normal+random_unit_vector();
            scattered=ray(rec.p,scatter_direction,r_in.time());
            attenuation=albedo->value(rec.u,rec.v,rec.p);
            return true;
        }
    public:
        shared_ptr<texture> albedo;
};

class metal:public material{
    public:
        metal(const shared_ptr<texture> a,double f):albedo(a),fuzz(f){}
		
        metal(const color a,double f):albedo(make_shared<solid_color>(a)),fuzz(f){}

        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered
        )const{
            vec3 reflected=reflect(unit_vector(r_in.direction()),rec.normal);
            scattered =ray(rec.p,reflected+fuzz*random_in_unit_sphere());
            attenuation=albedo->value(rec.u,rec.v,rec.p);
            return (dot(scattered.direction(),rec.normal)>0);
        }
    public:
		shared_ptr<texture> albedo;
        double fuzz;
};

class dielectric:public material{
    public:
        dielectric(double ri):ref_idx(ri){}

        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered
        )const{
            attenuation=color(1.0,1.0,1.0);
            double etai_over_etat=rec.front_face?(1.0/ref_idx):ref_idx;
            
            vec3 unit_direction=unit_vector(r_in.direction());
            double cos_theta=fmin(dot(-unit_direction,rec.normal),1.0);
            double sin_theta=sqrt(1.0-cos_theta*cos_theta);
            double reflect_prob=schlick(cos_theta,etai_over_etat);

            if (etai_over_etat*sin_theta>1.0||random_double()<reflect_prob){
                vec3 reflected=reflect(unit_direction,rec.normal);
                scattered=ray(rec.p,reflected);
                return true;
            }
            

            vec3 refracted=refract(unit_direction,rec.normal,etai_over_etat);
            scattered=ray(rec.p,refracted);
            return true;
        }
    public:
        double ref_idx;
};

class diffuse_light : public material {
	public:
		diffuse_light(shared_ptr<texture> a) :emit(a) {}
		diffuse_light(color a) :emit(make_shared<solid_color>(a)) {}

		virtual bool scatter(
			const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
		)const {
			return false;
		};

		virtual color emitted(double u, double v, const point3& p)const {
			return emit->value(u, v, p);
		}

	public:
		shared_ptr<texture> emit;
};

class isotropic :public material {
public:
	isotropic(shared_ptr<texture> a) :albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	)const {
		scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
public:
	shared_ptr<texture> albedo;
};

#endif