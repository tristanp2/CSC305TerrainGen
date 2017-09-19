#define _USE_MATH_DEFINES
#include "heightgenerator.h"
#include <random>
#include <math.h>

HeightGenerator::HeightGenerator()
{
	this->AMPLITUDE = 15.0f;
}

HeightGenerator::~HeightGenerator()
{
}

float HeightGenerator::generate_height(int x, int z)
{
	float total = get_interpolated_noise(x / 4.0f, z / 4.0f) * AMPLITUDE;
	total += get_interpolated_noise(x / 2.0f, z / 2.0f) * AMPLITUDE / 3.0f;
	total += get_interpolated_noise(x, z) * AMPLITUDE / 9.0f;
	return total;
}

float HeightGenerator::interpolate(float a, float b, float blend)
{
	float theta = blend * M_PI;
	float f = (1.0f - cos(theta)) * 0.5f;
	return a*(1.0f - f) + b*f;
}

//Numbers were just chosen arbitrarily
float HeightGenerator::get_noise(int x, int z)
{
	std::pair<int, int> p(x, z);
	std::map<std::pair<int, int>, float>::iterator it;
	float result;
	if ((it = noisy_heights.find(p)) != noisy_heights.end()) {
		result = it->second;
	}
	else {
		generator.seed(x * 17 + z * 23);
		result = dist(generator);
		noisy_heights.insert(std::pair<std::pair<int, int>, float>(p, result));
	}	
	return result;
}
//basically just a blur filter
float HeightGenerator::smooth_noise(int x, int z)
{
	std::pair<int, int> p(x, z);
	std::map<std::pair<int, int>, float>::iterator it;
	float result;
	if ((it = smooth_heights.find(p)) != smooth_heights.end()) {
		result = it->second;
	}
	else {
		float corners = (get_noise(x - 1, z - 1) + get_noise(x - 1, z + 1) + get_noise(x + 1, z - 1) + get_noise(x + 1, z + 1)) / 16.0f;
		float sides = (get_noise(x - 1, z) + get_noise(x, z - 1) + get_noise(x + 1, z) + get_noise(x, z + 1)) / 8.0f;
		float center = get_noise(x, z) / 4.0f;
		result = corners + center + sides;
		smooth_heights.insert(std::pair<std::pair<int, int>, float>(p, result));
	}
	return result;
}

float HeightGenerator::get_interpolated_noise(float x, float z)
{
	int x_int = int(x);
	int z_int = int(z);
	float frac_x = x - x_int;
	float frac_z = z - z_int;
	
	float f1 = smooth_noise(x_int, z_int);
	float f2 = smooth_noise(x_int + 1, z_int);
	float f3 = smooth_noise(x_int, z_int + 1);
	float f4 = smooth_noise(x_int + 1, z_int + 1);
	float fi1 = interpolate(f1, f2, frac_x);
	float fi2 = interpolate(f3, f4, frac_x);

	return interpolate(fi1, fi2, frac_z);
}
