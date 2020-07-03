#ifndef MATERIAL_H
#define MATERIAL_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "texture.h"
#include "onb.h"

class material{
    public:
        virtual bool scatter(
            const ray& r_in,const hit_record& rec,color& attenuation,ray& scattered,double& pdf
		)const{
			return false;
		}
		virtual double scattering_pdf(
			const ray& r_in, const hit_record& rec, const ray& scattered
		)const {
			return 0;
		}
		virtual color emitted(const hit_record& rec)const {
			
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
            const ray& r_in,const hit_record& rec,color& alb,ray& scattered,double& pdf
        )const{
			onb uvw;
			uvw.build_from_w(rec.normal);
			auto direction = uvw.local(random_cosine_direction());
            scattered=ray(rec.p,unit_vector(direction),r_in.time());
			alb =albedo->value(rec.u,rec.v,rec.p);
			pdf = dot(uvw.w(), scattered.direction()) / pi;
            return true;
        }
		double scattering_pdf(
			const ray& ray_in, const hit_record& rec, const ray& scattered
		) const{
			auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
			return cosine < 0 ? 0 : cosine / pi;
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

		virtual color emitted(const hit_record& rec)const {
			if (rec.front_face)
			return emit->value(rec.u, rec.v, rec.p);
			return color(0, 0, 0);
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