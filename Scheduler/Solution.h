#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <sstream>
#include "DataContainer.h"
#include "Job.h"
#include "Machine.h"
#include "Resource.h"

using namespace std;

class Solution
{
private:
	int cost;
	bool savedFlag = false;
	DataContainer* data;
	vector<Job*> jobs;
	vector<Machine*> machines;
	vector<Resource*> resources;
	unordered_map<Job*, int*> saved;
	unordered_map<Machine*, list<Job*>> savedSchedule;
	unordered_map<Resource*, multiset<Instant*, Instant::InstantComparator>> savedResources;
	int savedCost;
	unordered_map<Job*, int*> savedTemp;
	unordered_map<Machine*, list<Job*>> savedScheduleTemp;
	int tempSavedCost = INT_MAX;
public:

	Solution(DataContainer* data);
	~Solution();

	Machine* getMachineById(int id);
	int jobCount();
	int calcCost();
	void improveResources();
	void randomSchedule();
	void smartRandomSchedule();
	void dueDateBasedSchedule();
	void readyDateBasedSchedule();
	void resourceUsedBasedSchedule();
	void schedule();
	void reset();
	void printSchedule();
	bool relaxMachinesCosts();
	bool relaxMachinesTimes();
	void randomJobSwapOnMachine();
	void randomJobSwapBetweenMachines();
	void print(ostream& out);
	void swapJobsOnMachine();
	void partialShuffle();
	void localSearch();
	void localSearchNoised();
	void tardinessFix();
	void swapJobsOnMachine(int iterations);
	void setupTest();
	void printGraph();
	void printResourceSchedulingGraph();
	void graph(string filePath, string jsData);
	void save();
	void load();
	void saveTemp();
	void loadTemp();
	int getTempCost();
	void store();
};

