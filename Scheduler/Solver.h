#pragma once
#include "Solution.h"

class Solver
{
private:
	Solution* solution;
public:
	Solver(Solution* solution);
	~Solver();

	void randomStep();
};

