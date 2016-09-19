#pragma once

#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>

using namespace std;

class RandomGenerator {
private:

	random_device rd;
	default_random_engine gen;
	uniform_real_distribution<> real_distribution;

protected:
	RandomGenerator();

public:
	static RandomGenerator& getInstance();

	RandomGenerator(RandomGenerator const&) = delete;             // Copy construct
	RandomGenerator(RandomGenerator&&) = delete;                  // Move construct
	RandomGenerator& operator=(RandomGenerator const&) = delete;  // Copy assign
	RandomGenerator& operator=(RandomGenerator&&) = delete;       // Move assign

	default_random_engine& getRandomEngine();
	int randomInt(int from, int to);
	double randomDouble();

};