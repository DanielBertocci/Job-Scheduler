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
	int newCost;
	while (elapsed_ms < time && cost > 0) {
		newCost = solver->improve();
		if (newCost < cost) {
			cost = newCost;
			cout << "Current best: " << cost << endl;
			//solver->save();
		}
		solver->updateDecay();

		end = chrono::system_clock::now();
		elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	}

	int finalCost = solver->storeSolution();

	ofstream out("output.csv", ofstream::out | ofstream::app);
	out << data->getFile() << ";" << elapsed_ms << " ms;" << cost << endl;
	out.close();

	// Save 2 file with graph of solution and resource usage.
	if (verbose == true) {
		solver->storeSolutionGraphs();
	}
	cout << elapsed_ms << " " << finalCost;
	return 0;
}