#pragma once
#include "DataContainer.h"
#include "Solution.h"
#include <functional>
#include <random>

using namespace std;

class Solver
{
private:
	Solution* solution;
	int changeSolutionIfImprove(function<void()> algorithm);
	double decay = 1;
	double decayFactor = 0.99;
	bool accept(int oldCost, int newCost);

public:
	Solver(DataContainer* data);
	~Solver();

	int improve();
	void updateDecay();
	int smartImprove();
	int testSolver();
	void save();
	void disturb();
	void storeSolution();
	void storeSolutionGraphs();
	int exploration();
	int search();
};

