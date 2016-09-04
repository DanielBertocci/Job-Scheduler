#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include "Job.h"

using namespace std;

class Job;

class Machine
{
private:
	int id;
	int time = 0;
	int cost = 0;
	bool scheduled = false;
	unordered_map<Job*,int> processingTime;
	unordered_map<Job*, unordered_map<Job*, int>> setupTime;
	list<Job*> previousScheduledJobs;
	list<Job*> scheduledJobs;
	void calcCostTo(list<Job*>::iterator iterator);
	void scheduleFrom(list<Job*>::iterator iterator);
	void resetJobResourcesFrom(list<Job*>::iterator iterator);
	list<Job*> getJobSchedulableOnMachine(Machine* machine);
	int getStartForAllResources(Job* job, int start);

public:
	Machine(
		int id,
		unordered_map<Job*, int> processingTime,
		unordered_map<Job*, unordered_map<Job*, int>> setupTime
	);
	~Machine();

	int getId();
	int getCost();
	bool addJob(Job* job);
	void schedule();
	Job* getLastScheduledJob();
	void printSchedule();
	void resetJobResources();
	void reset();
	void shiftScheduledByDueDate();
	void randomJobSwap();
	void improveSetup();
	void improveTryAllSwap();
	void storeCurrentScheduling();
	bool swapRandomJobToMachine(Machine* machine);
	bool sendFirstAvailableJobToMachine(Machine* machine);
	void previousSchedule();
};

