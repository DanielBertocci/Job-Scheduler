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
	DataContainer* data;
	vector<Job*> jobs;
	vector<Machine*> machines;
	vector<Resource*> resources;
	unordered_map<Job*, int*> saved;
public:

	Solution(DataContainer* data);
	~Solution();

	Machine* getMachineById(int id);
	int calcCost();
	void randomSchedule();
	void dueDateBasedSchedule();
	void readyDateBasedSchedule();
	void resourceUsedBasedSchedule();
	void schedule();
	void reset();
	void printSchedule();
	bool relaxMachines();
	void randomJobSwapBetweenMachines();
	void print(ostream& out);
	void swapJobsOnMachine();
	void partialShuffle();
	void tardinessFix();
	void swapJobsOnMachine(int iterations);
	void setupTest();
	void printGraph();
	void printResourceSchedulingGraph();
	void graph(string filePath, string jsData);
	void save();
	void load();
};

