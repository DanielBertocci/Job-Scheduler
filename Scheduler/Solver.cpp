#include <random>
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
	//this->solution->smartRandomSchedule();
	this->solution->schedule();
	cout << "Start cost: " << this->solution->calcCost() << endl;
}

Solver::~Solver()
{
}

int Solver::improve()
{
	this->solution->saveTemp();

	this->solution->localSearch();

	if (!this->accept(this->solution->getTempCost(), this->solution->calcCost())) {
		this->solution->loadTemp();
	}

	this->solution->saveTemp();

	for (int i = 0; i < 5; ++i) {
		this->solution->randomJobSwapBetweenMachines();
	}
	this->solution->localSearch();

	if (!this->accept(this->solution->getTempCost(), this->solution->calcCost())) {
		this->solution->loadTemp();
	}

	this->solution->saveTemp();

	this->solution->localSearchNoised();

	if (!this->accept(this->solution->getTempCost(), this->solution->calcCost())) {
		this->solution->loadTemp();
	}

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

	this->solution->save();

	this->solution->tardinessFix();

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

int Solver::testSolver()
{
	int cost = this->solution->calcCost();
	int nCost;

	this->solution->saveTemp();

	this->solution->localSearch();

	nCost = this->solution->calcCost();

	if (nCost <= 0) return 0;
	if (this->accept(cost, nCost)) {
		cost = nCost;
	}
	else {
		this->solution->loadTemp();
	}

	this->solution->save();

	this->solution->relaxMachinesCosts();
	this->solution->localSearch();

	nCost = this->solution->calcCost();

	if (nCost <= 0) return 0;
	if (this->accept(cost, nCost)) {
		cost = nCost;
	}
	else {
		this->solution->loadTemp();
	}

	this->solution->saveTemp();

	this->solution->localSearchNoised();

	nCost = this->solution->calcCost();

	if (nCost <= 0) return 0;
	if (this->accept(cost, nCost)) {
		cost = nCost;
	}
	else {
		this->solution->loadTemp();
	}

	this->updateDecay();
	return this->solution->calcCost();
}

void Solver::save()
{
	this->solution->save();
}

void Solver::disturb()
{
	for (int i = 0; i < this->solution->jobCount() / 2; ++i) {
		if (rand() % 2 == 0) {
			this->solution->randomJobSwapBetweenMachines();
		}
		else {
			if (rand() % 2 == 0) {
				this->solution->relaxMachinesCosts();
			}
			else {
				this->solution->relaxMachinesTimes();
			}
			this->solution->localSearch();
			this->solution->localSearchNoised();
		}
	}
}

void Solver::storeSolution()
{
	this->solution->load();
	this->solution->store();
}

void Solver::storeSolutionGraphs()
{
	this->solution->printGraph();
	this->solution->printResourceSchedulingGraph();
}

int Solver::exploration()
{
	random_device rd;
	mt19937 gen(rd());

	for (int i = 0; i < this->solution->jobCount() / 2; ++i) {
		uniform_real_distribution<> dis(0, 1);

		this->solution->saveTemp();

		if (dis(gen) < 0.5) {
			this->solution->randomJobSwapOnMachine();
		}
		else {
			this->solution->randomJobSwapBetweenMachines();
		}

		if (!this->accept(this->solution->getTempCost(), this->solution->calcCost())) {
			this->solution->loadTemp();
		}

		return this->solution->calcCost();
	}
}

int Solver::search()
{
	this->solution->saveTemp();

	this->solution->localSearch();

	if (!this->accept(solution->getTempCost(), this->solution->calcCost())) {
		this->solution->loadTemp();
	}

	return this->solution->calcCost();
}
