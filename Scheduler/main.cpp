#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include "DataContainer.h"
#include "Solution.h"

using namespace std;

int main(int argc, char **argv) {
	/*if (argc < 3) {
		cout << "Error, arguments missing!\n";
		cout << "usage: " << argv[0] << " <input file> <time> [outputDir]" << endl;
		return -1;
	}*/

	//string inputFile = argv[1];
	string inputFile = "I_30_2_02_02_2.csv";
	DataContainer *data = DataContainer::fromFile("C:/Users/Daniel/Desktop/" + inputFile);
	Solution* solution = new Solution(data);
	//Solver* solver = new Solver(solution);

	solution->randomSchedule();
	solution->schedule();

	chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	long elapsed_ms = 0;

	int cost = INT_MAX;
	int newCost = INT_MAX;
	int bestCounter = 0;

	while (elapsed_ms < 7000 && cost > 0) {
		solution->swapJobsOnMachine();
		cout << "Soluzione swap:\t\t" << solution->calcCost() << endl;

		newCost = solution->calcCost();
		if (newCost < cost) {
			cost = newCost;
			solution->save();
			if (cost <= 0) break;
		}

		solution->relaxMachines();
		cout << "Soluzione relaxed:\t" << solution->calcCost() << endl;

		newCost = solution->calcCost();
		if (newCost < cost) {
			cost = newCost;
			solution->save();
			if (cost <= 0) break;
		}

		solution->randomJobSwapBetweenMachines();
		cout << "Soluzione machine swap:\t" << solution->calcCost() << endl;

		newCost = solution->calcCost();
		if (newCost < cost) {
			cost = newCost;
			solution->save();
			if (cost <= 0) break;
		}

		if (newCost == cost) {
			bestCounter++;
		}

		end = chrono::system_clock::now();
		elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	}

	solution->load();
	ofstream out("C:/Users/Daniel/Desktop/" + inputFile.substr(0, inputFile.size() - 4) + "_sol.csv");
	solution->print(out);
	out.close();
	solution->printGraph();
	solution->printResourceSchedulingGraph();

	cout << "Best result: " << cost << endl;
	cout << "Best counter: " << bestCounter;
	int i;
	cin >> i;
	return 0;
}