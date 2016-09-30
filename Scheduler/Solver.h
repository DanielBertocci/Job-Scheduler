#pragma once
#include "DataContainer.h"
#include "Solution.h"

using namespace std;

class Solver
{
private:
	double decay = 1;
	double decayFactor = 0.9;
	int solveBetterCounter = 0;
	bool accept(int oldCost, int newCost);

public:
	Solver(DataContainer* data);
	~Solver();
	Solution* solution;

	int improve();
	int scheduleByMachineId(int machineId);
	int storeSolution();
	void save();
	void load();
	void saveBest();
	int tryAlgorithm();
	void loadBest();
	void storeSolutionGraphs();
	void updateDecay();
};

