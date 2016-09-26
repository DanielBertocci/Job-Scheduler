#include <random>
#include "Solver.h"

bool Solver::accept(int oldCost, int newCost)
{
	if (newCost < oldCost) return true;
	double score = exp(-(newCost - oldCost) / decay);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);

	double random = dis(gen);

	return random <= score;
}

void Solver::updateDecay()
{
	this->decay *= this->decayFactor;
}

Solver::Solver(DataContainer * data)
{
	this->solution = new Solution(data);
	this->solution->randomSchedule();
	this->solution->schedule();
	cout << "Start cost: " << this->solution->calcCost() << endl;
}

Solver::~Solver()
{
}

int Solver::improve()
{
	if (RandomGenerator::getInstance().randomDouble() < 0.5) {
		this->solution->relaxMachinesCosts();
	}
	else {
		this->solution->randomJobSwapBetweenMachines();
	}
	this->solution->localSearch();
	//this->solution->saveBest();
	/*this->solution->localSearch();

	this->saveTempBetter();

	this->solution->relaxMachinesCosts();
	this->solution->localSearch();

	this->saveTempBetter();

	double randomCheck = RandomGenerator::getInstance().randomDouble();
	if (randomCheck < 0.7) {
		this->solution->randomJobSwapBetweenMachines();
		this->improve();
	}
	else if (randomCheck < (1 - log(this->solveBetterCounter / 500 + 1) / 10)) {
		this->solution->localSearchNoised();
	}
	else {
		this->solveBetterCounter = 0;
		this->solution->randomSchedule();
		this->solution->schedule();
		this->improve();
	}

	this->saveTempBetter();

	this->solution->loadTemp();
	*/
	return this->solution->calcCost();
}

void Solver::save()
{
	this->solution->save();
}

int Solver::storeSolution()
{
	//this->solution->load();
	//this->solution->localSearchCompressionFix();
	this->solution->store();
	return this->solution->calcCost();
}

void Solver::storeSolutionGraphs()
{
	this->solution->printGraph();
	this->solution->printResourceSchedulingGraph();
	this->solution->printResourceUsageGraph();
}

void Solver::saveTempBetter()
{
	if (this->solution->calcCost() < this->solution->getTempCost()) {
		this->solution->saveTemp();
		this->solveBetterCounter = 0;
	}
	else {
		this->solveBetterCounter++;
	}
}
