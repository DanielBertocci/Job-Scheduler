#pragma once

#include <list>
#include <random>
#include <unordered_map>
#include <iostream>
#include "Job.h"
#include "RandomGenerator.h"

using namespace std;

class Job;

class Machine
{
private:
	int id;
	int time = 0;
	int cost = 0;
	int previousCost;
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
	int getTime();
	int getJobProcessingTime(Job* job);
	int getSetupTime(Job* prev, Job* next);
	list<Job*> getSchedule();
	void setSchedule(list<Job*> schedule);
	bool addJob(Job* job);
	void schedule();
	Job* getLastScheduledJob();
	void printSchedule();
	void resetJobResources();
	void reset();
	void shiftScheduledByDueDate();
	void schedulingShuffle();
	void randomJobSwap();
	void partialShuffle();
	void expansiveJobReschedule();
	void improveSetup();
	void improveTryAllSwap();
	void storeCurrentScheduling();
	bool swapRandomJobToMachine(Machine* machine);
	bool sendFirstAvailableJobToMachine(Machine* machine);
	void previousSchedule();
	void bestScheduleForCurrentJobs();
};

