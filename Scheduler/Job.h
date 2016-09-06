#pragma once

#include <algorithm>
#include <unordered_map>
#include "Machine.h"
#include "Resource.h"

using namespace std;

class Machine;

class Job
{
private:
	int id;
	int readyDate;
	int dueDate;
	int penalty;
	unordered_map<Resource*, int> resources;
	unordered_map<Resource*, ResourceInterval* > intervals;

	int start;
	int end;
	Machine* machine;
public:

	Job(int id, int dueDate, int readyDate, int penalty, unordered_map<Resource*, int> resources);
	~Job();

	int getId() const;
	int getStart();
	int getEnd();
	int getReadyDate() const;
	int getDueDate() const;
	int getResourceCount() const;
	unordered_map<Resource*, int> getRequiredResources() const;
	Machine* getMachine();
	void reset();
	void resetIntervals();
	void setSchedule(int start, int processingTime);
	void setMachine(Machine* machine);
	int getCost();
	string toString();
	void addInterval(Resource* resource, ResourceInterval* interval);
	static bool startBefore(const Job* job1, const Job* job2);
	static bool dueDateBefore(const Job* job1, const Job* job2);
	static bool readyDateBefore(const Job* job1, const Job* job2);
	static bool moreUsedResourceBefore(const Job* job1, const Job* job2);
	static bool minorMachineIdBefore(const Job* job1, const Job* job2);
};
