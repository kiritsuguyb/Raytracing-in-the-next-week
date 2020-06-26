#pragma once
#include "rtweekend.h"
inline double trilinear_interp(double c[2][2][2], double u, double v, double w) {
	auto accum = 0.0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				//对于8个采样点中任意一个来说，其权重应该等于其对角的长方体的体积
				//trilinear本身没啥，就是3d空间中的线性采样
				//如果颠倒这个值，就会出现断崖式的渐变，也很有趣
				accum += (i*u + (1 - i)*(1 - u))*
					(j*v + (1 - j)*(1 - v))*
					(k*w + (1 - k)*(1 - w))*c[i][j][k];
			}
		}
	}
	return accum;
}
inline double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
	auto uu = u * u*(3 - 2 * u);
	auto vv = v * v*(3 - 2 * v);
	auto ww = w * w*(3 - 2 * w);

	auto accum = 0.0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				//对于8个采样点中任意一个来说，其权重应该等于其对角的长方体的体积
				//trilinear本身没啥，就是3d空间中的线性采样
				//如果颠倒这个值，就会出现断崖式的渐变，也很有趣
				vec3 weight_v(u - i, v - j, w - k);
				accum += (i*u + (1 - i)*(1 - u))*
					(j*v + (1 - j)*(1 - v))*
					(k*w + (1 - k)*(1 - w))*dot(c[i][j][k], weight_v);
			}
		}
	}
	return accum;
}

class perlin {
	public:
		perlin() {
			ranvec = new vec3[point_count];
			ranfloat = new double[point_count];

			for (int i = 0; i < point_count; ++i) {
				ranvec[i] = random_unit_vector();
				ranfloat[i] = random_double();
			}

			perm_x = perlin_generate_perm();
			perm_y = perlin_generate_perm();
			perm_z = perlin_generate_perm();
		}
		~perlin() {
			delete[] ranvec;
			delete[] ranfloat;
			delete[] perm_x;
			delete[] perm_y;
			delete[] perm_z;
		}

		double noise(const point3& p)const {
			auto u = p.x() - floor(p.x());
			auto v=p.y()-floor(p.y()); 
			auto w=p.z()-floor(p.z());

			int i=floor(p.x());
			int j=floor(p.y());
			int k=floor(p.z());
			vec3 c[2][2][2];

			for (int di = 0; di < 2; di++)
			{
				for (int dj = 0; dj < 2; dj++)
				{
					for (int dk = 0; dk < 2; dk++)
					{
						c[di][dj][dk] = ranvec[
							perm_x[(i+di)&255]^
							perm_y[(j+dj)&255]^
							perm_z[(k+dk)&255]
						];//在获取ranfloat值之前增加一层对物体坐标的随机重映射，
						  //自然不会有pattern的结果
					}
				}
			}

			return perlin_interp(c,u,v,w);//[-1,1]，均值为0
		}
		double pattern_noise(const point3& p)const {
			auto u = p.x() - floor(p.x());
			auto v = p.y() - floor(p.y());
			auto w = p.z() - floor(p.z());

			auto i=static_cast<int>(p.x())&255;
			auto j=static_cast<int>(p.y())&255;
			auto k=static_cast<int>(p.z())&255;

			//异或可以达到10进制中类似幂次的效果，负负得正，
			//这样使得结果不会塌缩到某些范围而仍然是相对均匀的
			//用来作为采样下标仍然可以得到有意义的结果
			return ranfloat[i^j ^k];
		}
		double turb(const point3& p, int depth = 7)const {
			auto accum = 0.0;
			auto temp_p = p;
			auto weight = 1.0;
			for (int i = 0; i < depth; i++)
			{
				accum += weight * noise(temp_p);
				weight *= 0.5;
				temp_p *= 2;
			}//均值仍然为0
			return fabs(accum);
			//返回绝对值则均值还是应该在[0.0.5],因为depth==1时方差较大，depth趋于无穷时方差趋于0.
		}
	private:
		static const int point_count = 256;
		vec3* ranvec;
		double* ranfloat;
		int* perm_x;
		int* perm_y;
		int* perm_z;

		static int* perlin_generate_perm() {
			auto p = new int[point_count];

			for (int i = 0; i < perlin::point_count; i++)
				p[i] = i;

			permute(p, point_count);
			return p;
		}

		static void permute(int* p, int n) {
			for (int i = n - 1; i > 0; i--) {
				//通过一次遍历，构建无序区，无序区逐渐扩大，有序区逐渐减小
				//每次随机获取有序区的数字存放到目标区域，使得无序区扩充
				//直观上来说类似洗扑克，但是更有规律
				//是排序的反过程
				//最后构建出来，只要给一个[0,255]之间的数，就能给一个[0,255]的随机数
				//但是模式是固定的，因此如果使用空间坐标采样，自然会有pattern出现。
				int target = random_int(0, i);
				int tmp = p[i];
				p[i] = p[target];
				p[target] = tmp;
			}
		}
		
};