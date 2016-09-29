#pragma once

#include <unordered_map>
#include "Machine.h"
#include "Resource.h"
#include "ScheduleList.h"

using namespace std;

class Machine;
class Instant;
class Resource;

// Types definition.
typedef unordered_map<Resource*, int> ResourceQuantityMap;
typedef multiset<Instant*>::iterator InstantIteratorSet;
typedef pair<InstantIteratorSet, InstantIteratorSet> InstantIteratorSetPair;
typedef unordered_map<Resource*, InstantIteratorSetPair> ResourceInstantPairIteratorMap;

class Job
{
private:
	int id;
	int readyDate;
	int dueDate;
	int penalty;

	// Required resources needed to execute the job.
	ResourceQuantityMap resources;
	ResourceInstantPairIteratorMap instants;

	int start;
	int end;
	Machine* machine;
public:
	Job(int id, int dueDate, int readyDate, int penalty, ResourceQuantityMap resources);
	~Job();

	struct CostComparator {
		bool operator()(Job* a, Job* b) const {
			return a->getCost() < b->getCost();
		}
	};

	typedef multiset<Job*, Job::CostComparator> JobSetStartOrder;
	typedef JobSetStartOrder::iterator JobSetStartOrderIterator;

	static JobSetStartOrder scheduling;
	JobSetStartOrderIterator schedulingPosition;
	list<Job*>::iterator schedulingMachinePosition;

	// Getters.
	int getCost();
	int getDueDate() const;
	int getEnd();
	int getId() const;
	int getPenalty() const;
	int getReadyDate() const;
	int getResourceCount() const;
	int getStart();
	Machine* getMachine();
	ResourceQuantityMap getRequiredResources() const;

	// Setters.
	void setMachine(Machine* machine);
	void setSchedule(int start, int processingTime);
	void setInstants(Resource* resource, InstantIteratorSetPair interval);

	// Resets.
	void reset();
	void resetInstants();

	// Utils methods.
	string toString();
	void shiftLeft(int amount);

	// Sort method. Used usually to put in order in <set> or <multiset>
	static bool dueDateBefore(const Job* job1, const Job* job2);
	static bool minorMachineIdBefore(const Job* job1, const Job* job2);
	static bool moreUsedResourceBefore(const Job* job1, const Job* job2);
	static bool readyDateBefore(const Job* job1, const Job* job2);
	static bool startBefore(const Job* job1, const Job* job2);
};