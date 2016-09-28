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
	this->solution->saveBest();
	cout << "Start cost: " << this->solution->calcCost() << endl;
}

Solver::~Solver()
{
}

int Solver::improve()
{
	int cost = this->solution->calcCost();
	for (int i = 0; i < this->solution->getJobCount() / 2; ++i) {
		if (RandomGenerator::getInstance().randomDouble() < 0.5) {
			if (RandomGenerator::getInstance().randomDouble() < 0.8) {
				this->solution->randomJobToAnotherMachine();
			}
			else {
				this->solution->relaxMachinesCosts();
			}
		}
		else {
			this->solution->randomJobSwapBetweenMachines();
		}
	}

	this->solution->localSearch();
	this->updateDecay();

	return this->solution->calcCost();
}

void Solver::save()
{
	this->solution->save();
}

void Solver::saveBest()
{
	this->solution->saveBest();
}

void Solver::loadBest()
{
	this->solution->loadBest();
}

int Solver::storeSolution()
{
	this->solution->loadBest();
	this->solution->localSearchCompressionFix();
	this->solution->store();
	return this->solution->calcCost();
}

void Solver::storeSolutionGraphs()
{
	this->solution->printGraph();
	this->solution->printResourceSchedulingGraph();
	this->solution->printResourceUsageGraph();
}
