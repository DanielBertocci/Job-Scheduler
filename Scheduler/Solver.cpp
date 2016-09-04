#include "Solver.h"

Solver::Solver(Solution * solution)
{
	solution->randomSchedule();
	this->solution = solution;
}

Solver::~Solver()
{
}

void Solver::randomStep()
{
	int cost = INT_MAX;
	int newCost = INT_MAX;

	solution->swapJobsOnMachine();
	//cout << "Soluzione swap:\t\t" << solution->calcCost() << endl;

	newCost = solution->calcCost();
	if (newCost < cost) {
		cost = newCost;
		solution->save();
		if (cost <= 0) return;
	}

	solution->relaxMachines();
	//cout << "Soluzione relaxed:\t" << solution->calcCost() << endl;

	newCost = solution->calcCost();
	if (newCost < cost) {
		cost = newCost;
		solution->save();
		if (cost <= 0) return;
	}

	solution->randomJobSwapBetweenMachines();
	//cout << "Soluzione machine swap:\t" << solution->calcCost() << endl;

	newCost = solution->calcCost();
	if (newCost < cost) {
		cost = newCost;
		solution->save();
		if (cost <= 0) return;
	}
}
