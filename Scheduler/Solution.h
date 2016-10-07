#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include "DataContainer.h"
#include "Job.h"
#include "Machine.h"
#include "Resource.h"

using namespace std;

typedef vector<Job*> JobVector;
typedef vector<Job*>::iterator JobVectorIterator;
typedef vector<Machine*> MachineVector;
typedef vector<Resource*> ResourceVector;
typedef unordered_map<Job*, int[3]> JobIdStartEndMap;
typedef unordered_map<Machine*, JobList> MachineScheduleMap;
typedef unordered_map<Resource*, InstantSet> ResourceInstantSetMap;

class Solution
{
private:
	bool savedFlag = false;
	DataContainer* data;
	double localSearchFactor = 0.5;
	int cost;
	int savedCost;
	int tempSavedCost = INT_MAX;
	JobIdStartEndMap saved;
	JobIdStartEndMap savedTemp;
	JobIdStartEndMap best;
	int bestCost = INT_MAX;
	JobVector jobs;
	MachineScheduleMap savedSchedule;
	MachineScheduleMap savedScheduleTemp;
	MachineVector machines;
	ResourceInstantSetMap savedResources;
	ResourceVector resources;
	unordered_map<Job*, vector<Machine*>> machineForJob;
public:
	int seed;
	int iterations;

	Solution(DataContainer* data);
	~Solution();

	// Getters.
	int getJobCount();
	int getSavedCost();
	int getTempCost();
	int getBestCost();
	Machine* getMachineById(int id);

	// Resets.
	void reset();

	// Save & Load.
	void load();
	void save();
	bool saveBest();
	void loadBest();

	// Print & store.
	void graph(string filePath, string jsData);
	void graphArea(string filePath, string jsData);
	void print(ostream& out);
	void printBest(ofstream& out);
	void printGraph();
	void printResourceSchedulingGraph();
	void printResourceUsageGraph();
	void store();
	void storeBest();

	// Algorithms
	bool relaxMachinesCosts();
	bool relaxMachinesTimes();
	int calcCost();
	void improveResources();
	void localSearch();
	void localSearchCompressionFix();
	void localSearchNoised();
	void randomJobSwapBetweenMachines();
	void randomJobSwapOnMachine();
	void randomJobToAnotherMachine();
	void randomSchedule();
	void removeIdlesFromBest();
	void schedule();
	void smartRandomSchedule();
	void removeIdleFromMachines();
	void moveWorstJob();
	void sendToBetterProcessing();
};

