#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>

class PerlinNoise {
	std::vector<int> p;

public:
	PerlinNoise();

	PerlinNoise(unsigned int seed);

	float noise(float x, float y, float z = 0);

private:
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y, float z);
};

#endif

