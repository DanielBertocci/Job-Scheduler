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
	string inputFile = "I_100_3_02_10_4.csv";
	int time = 36000;

	DataContainer *data = DataContainer::fromFile("C:/Users/Daniel/Desktop/" + inputFile);
	Solution* solution = new Solution(data);
	//Solver* solver = new Solver(solution);

	solution->randomSchedule();
	solution->schedule();
	cout << "Start cost: " << solution->calcCost() << endl;

	chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	long elapsed_ms = 0;

	int cost = INT_MAX;
	int newCost = INT_MAX;
	int bestCounter = 0;

	while (elapsed_ms < time && cost > 0) {

		// Ottimo per job piccoli
		/////////////////////////////////////////////

		solution->swapJobsOnMachine();
		if (solution->calcCost() <= 0) {
			//solution->printGraph();
			solution->save();
			break;
		};

		solution->save();
		int tCost = solution->calcCost();

		solution->relaxMachines();
		solution->randomJobSwapBetweenMachines();
		solution->swapJobsOnMachine();

		int nCost = solution->calcCost();

		if (nCost > tCost) {
			solution->load();
		}
		else {
			cost = nCost;
		}
		if (solution->calcCost() <= 0) {
			//solution->printGraph();
			solution->save();
			break;
		};


		if (rand() % 4 == 0) {
			solution->tardinessFix();
			if (solution->calcCost() <= 0) {
				//solution->printGraph();
				solution->save();
				break;
			};
		}

		///////////////////////////////////////////////////////////////////////////

		/*solution->randomJobSwapBetweenMachines();
		solution->swapJobsOnMachine(10);
		solution->randomJobSwapBetweenMachines();
		solution->randomJobSwapBetweenMachines();
		solution->swapJobsOnMachine(3);

		solution->relaxMachines();
		newCost = solution->calcCost();
		if (newCost < cost) {
			cost = newCost;
			if (cost <= 0) break;
		}*/


		// IGNORANTE
		//////////////////

		//solution->swapJobsOnMachine();
		////cout << "Soluzione swap:\t\t" << solution->calcCost() << endl;

		//newCost = solution->calcCost();
		//if (newCost < cost) {
		//	cost = newCost;
		//	solution->save();
		//	if (cost <= 0) break;
		//}

		//solution->relaxMachines();
		////cout << "Soluzione relaxed:\t" << solution->calcCost() << endl;

		//newCost = solution->calcCost();
		//if (newCost < cost) {
		//	cost = newCost;
		//	solution->save();
		//	if (cost <= 0) break;
		//}

		//solution->randomJobSwapBetweenMachines();
		////cout << "Soluzione machine swap:\t" << solution->calcCost() << endl;

		//newCost = solution->calcCost();
		//if (newCost < cost) {
		//	cost = newCost;
		//	solution->save();
		//	if (cost <= 0) break;
		//}

		end = chrono::system_clock::now();
		elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	}

	solution->load();
	ofstream out("C:/Users/Daniel/Desktop/" + inputFile.substr(0, inputFile.size() - 4) + "_sol.csv");
	solution->print(out);
	out.close();
	solution->printGraph();
	solution->printResourceSchedulingGraph();

	cout << "Best result: " << solution->calcCost() << endl;
	int i;
	cin >> i;
	return 0;
}