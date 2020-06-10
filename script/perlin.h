#pragma once
#include "rtweekend.h"
class perlin {
	public:
		perlin() {
			ranfloat = new double[point_count];
			for (int i = 0; i < point_count; ++i) {
				ranfloat[i] = random_double();
			}

			perm_x = perlin_generate_perm();
			perm_y = perlin_generate_perm();
			perm_z = perlin_generate_perm();
		}
		~perlin() {
			delete[] ranfloat;
			delete[] perm_x;
			delete[] perm_y;
			delete[] perm_z;
		}

		double noise(const point3& p)const {
			auto u = p.x() - floor(p.x());
			auto v=p.y()-floor(p.y()); 
			auto w=p.z()-floor(p.z());

			auto i=static_cast<int>(4*p.x())&255;
			auto j=static_cast<int>(4*p.y())&255;
			auto k=static_cast<int>(4*p.z())&255;

			return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
		}
		double pattern_noise(const point3& p)const {
			auto u = p.x() - floor(p.x());
			auto v = p.y() - floor(p.y());
			auto w = p.z() - floor(p.z());

			auto i=static_cast<int>(4*p.x())&255;
			auto j=static_cast<int>(4*p.y())&255;
			auto k=static_cast<int>(4*p.z())&255;

			return ranfloat[i|j |k];
		}
	private:
		static const int point_count = 256;
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