#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include "Job.h"
#include "RandomGenerator.h"

using namespace std;

class Job;

typedef unordered_map<Job*, int> JobProcessingTimeMap;
typedef unordered_map<Job*, unordered_map<Job*, int>> JobSetupTimeMap;
typedef list<Job*> JobList;
typedef list<Job*>::iterator JobListIterator;

class Machine
{
private:
	int id;
	int time = 0;
	int cost = 0;
	int previousCost;
	bool scheduled = false;
	JobProcessingTimeMap processingTime;
	JobSetupTimeMap setupTime;
	JobList previousScheduledJobs;
	JobList scheduledJobs;
	void calcCostTo(JobListIterator iterator);
	void scheduleFrom(JobListIterator iterator);
	void resetJobResourcesFrom(JobListIterator iterator);
	list<Job*> getJobSchedulableOnMachine(Machine* machine);
	int getStartForAllResources(Job* job, int start);
	void swapJobs(JobListIterator j1, JobListIterator j2);
	bool canSwapImproveCost(JobListIterator j1, JobListIterator j2);

public:
	Machine(
		int id,
		JobProcessingTimeMap processingTime,
		JobSetupTimeMap setupTime
	);
	~Machine();

	// Getters.
	int getId();
	int getCost();
	int getTime();
	int getJobProcessingTime(Job* job);
	int getSetupTime(Job* prev, Job* next);
	JobList getSchedule();

	// Setters.
	void setSchedule(JobList schedule);

	// Resets.
	void reset();
	void resetJobResources();

	// Utils.
	bool addJob(Job* job);
	bool addJobFront(Job* job);
	bool sendFirstAvailableJobToMachine(Machine* machine);
	bool swapRandomJobToMachine(Machine* machine);
	void bestScheduleForCurrentJobs();
	void improveTryAllSwap();
	void previousSchedule();
	void randomJobSwap();
	void schedule();
	void schedulingShuffle();
};

