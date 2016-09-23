#pragma once
#include <iostream>
#include <vector>
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
typedef unordered_map<Job*, int*> JobIdStartEndMap;
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
	JobVector jobs;
	MachineScheduleMap savedSchedule;
	MachineScheduleMap savedScheduleTemp;
	MachineVector machines;
	ResourceInstantSetMap savedResources;
	ResourceVector resources;

public:

	Solution(DataContainer* data);
	~Solution();

	// Getters.
	int getJobCount();
	int getTempCost();
	Machine* getMachineById(int id);

	// Resets.
	void reset();

	// Save & Load.
	void load();
	void loadTemp();
	void save();
	void saveTemp();

	// Print & store.
	void graph(string filePath, string jsData);
	void print(ostream& out);
	void printGraph();
	void printResourceSchedulingGraph();
	void store();

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
	void randomSchedule();
	void schedule();
	void smartRandomSchedule();
};

