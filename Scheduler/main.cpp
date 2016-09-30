#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include "DataContainer.h"
#include "Solver.h"

using namespace std;

int main(int argc, char **argv) {
	bool verbose = false;

	// Parameters setup.
	if (argc < 3) {
		cout << "Error, arguments missing!\n";
		cout << "usage: " << argv[0] << " <input file> <time> [-v]" << endl;
		return -1;
	}

	string inputFile = argv[1];
	int time = atoi(argv[2]) * 1000;

	// Check for verbose option.
	if (argc == 4) {
		verbose = true;
	}

	DataContainer *data = DataContainer::fromFile(inputFile);
	Solver* solver = new Solver(data);

	// Time setup.
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	long elapsed_ms = 0;

	// Search solutions.
	int cost = INT_MAX;
	int newCost = cost;
	int counter = 0;
	try
	{
		while (elapsed_ms < time && cost > 0) {
			counter++;
			newCost = solver->improve();
			if (newCost < cost) {
				cost = newCost;
				cout << "Current best: " << cost << endl;
			}
			if (cost == 0) {
				solver->solution->schedule();
			}

			end = chrono::system_clock::now();
			elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
		}
	}
	catch (...)
	{
		try
		{
			solver->loadBest();
		}
		catch (...)
		{
			solver = new Solver(data);
			solver->improve();
		}
	}

	int finalCost;

	try
	{
		finalCost = solver->storeSolution();
		cout << elapsed_ms << " " << finalCost << " ";
	}
	catch (...)
	{
		try
		{
			solver->solution->loadBest();
			finalCost = solver->solution->calcCost();
			cout << elapsed_ms << " " << finalCost << " ";
			solver->solution->store();
		}
		catch (...)
		{
			solver = new Solver(data);
			solver->improve();
		}
	}
	try
	{
		ofstream out("output.csv", ofstream::out | ofstream::app);
		out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		out << data->getFile() << ";" << elapsed_ms << " ms;" << finalCost << endl;
		out.close();
	}
	catch (...)
	{
		ofstream out("output.csv", ofstream::out | ofstream::app);
		out << data->getFile() << ";" << elapsed_ms << " ms;" << finalCost << endl;
		out.close();
	}

	// Save 2 file with graph of solution and resource usage.
	if (verbose == true) {
		solver->storeSolutionGraphs();
	}

	int option = 1;
	int machine = -1;
	while (option > 0) {
		cout << endl << "Option: ";
		cin >> option;
		switch (option) {
		case 1:
			cout << "Machine: ";
			cin >> machine;
			cout << "New cost: " << solver->scheduleByMachineId(machine) << endl;
			break;
		case 2:
			solver->save();
			cout << "Saved." << endl;
			break;
		case 3:
			solver->load();
			solver->storeSolutionGraphs();
			cout << "Loaded." << endl;
			break;
		case 4:
			cout << "New cost: " << solver->tryAlgorithm();
			cout << "Tryed." << endl;
			break;

		}
	}
	return 0;
}