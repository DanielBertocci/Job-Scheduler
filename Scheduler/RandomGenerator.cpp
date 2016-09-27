#include "RandomGenerator.h"

RandomGenerator::RandomGenerator() {
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	gen = default_random_engine(seed);
}

RandomGenerator& RandomGenerator::getInstance() {
	static RandomGenerator randomUtil;
	return randomUtil;
}

default_random_engine& RandomGenerator::getRandomEngine()
{
	return this->gen;
}

int RandomGenerator::randomInt(int from, int to) {
	uniform_int_distribution<> dis(from, to);
	return dis(gen);
}

double RandomGenerator::randomDouble() {
	return real_distribution(gen);
}
