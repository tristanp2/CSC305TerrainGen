#pragma once
#include <random>
#include <map>

class HeightGenerator
{
	//Following two maps are used to avoid redundant calculations and calls
	//Stores un-smoothed height values
	std::map<std::pair<int, int>, float> noisy_heights;
	//Stores smoothed height values
	std::map<std::pair<int, int>, float> smooth_heights;
	float AMPLITUDE;
	std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(-1.0f,1.0f);
	std::default_random_engine generator;
	float interpolate(float a, float b, float blend);
	float get_noise(int x, int z);
	float smooth_noise(int x, int z);
	float get_interpolated_noise(float x, float z);
public:
	HeightGenerator();
	~HeightGenerator();
	float generate_height(int x, int z);
};

