#pragma once
#include "DataContainer.h"
#include "Solution.h"
#include <functional>

using namespace std;

class Solver
{
private:
	Solution* solution;
	int changeSolutionIfImprove(function<void()> algorithm);

public:
	Solver(DataContainer* data);
	~Solver();

	int improve();
	int smartImprove();
	void storeSolution();
	void storeSolutionGraphs();
};

