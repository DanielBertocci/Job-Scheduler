#include "Solver.h"

int Solver::changeSolutionIfImprove(function<void()> algorithm)
{
	int cost = this->solution->calcCost();
	this->solution->save();
	algorithm();

	int nCost = this->solution->calcCost();

	if (nCost < cost) {
		cost = nCost;
	}
	else {
		this->solution->load();
	}

	return this->solution->calcCost();
}

Solver::Solver(DataContainer * data)
{
	this->solution = new Solution(data);
	this->solution->randomSchedule();
	//this->solution->smartRandomSchedule();
	this->solution->schedule();
	cout << "Start cost: " << this->solution->calcCost() << endl;
}

Solver::~Solver()
{
}

int Solver::improve()
{
	this->solution->randomJobSwapOnMachine();
	this->solution->relaxMachinesCosts();
	this->solution->localSearch();
	
	return this->solution->calcCost();
}

int Solver::smartImprove()
{
	int cost = this->solution->calcCost();
	int nCost;

	this->solution->save();

	this->solution->randomJobSwapOnMachine();
	this->solution->randomJobSwapBetweenMachines();
	this->solution->localSearch();

	nCost = this->solution->calcCost();

	if (nCost <= 0) return 0;
	if (nCost < cost) {
		cost = nCost;
	}
	else {
		this->solution->load();
	}

	this->solution->save();

	if (rand() % 2 == 0) {
		this->solution->relaxMachinesCosts();
	}
	else {
		this->solution->relaxMachinesTimes();
	}
	this->solution->localSearch();

	nCost = this->solution->calcCost();

	if (nCost <= 0) return 0;
	if (nCost < cost) {
		cost = nCost;
	}
	else {
		this->solution->load();
	}

	return this->solution->calcCost();
	/*int cost = this->solution->calcCost();
	int nCost;

	this->solution->save();

	this->solution->randomJobSwapOnMachine();
	this->solution->randomJobSwapBetweenMachines();
	this->solution->swapJobsOnMachine();

	nCost = this->solution->calcCost();

	if (nCost < cost) {
		cost = nCost;
	}
	else {
		this->solution->load();
	}

	this->solution->save();

	do {
		nCost = this->solution->calcCost();
		this->solution->randomJobSwapBetweenMachines();
	} while (this->solution->calcCost() < nCost);

	if (nCost < cost) {
		cost = nCost;
	}
	else {
		this->solution->load();
	}

	return this->solution->calcCost();*/
}

void Solver::storeSolution()
{
	this->solution->store();
}

void Solver::storeSolutionGraphs()
{
	this->solution->printGraph();
	this->solution->printResourceSchedulingGraph();
}
