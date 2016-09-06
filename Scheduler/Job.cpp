#include "Job.h"

Job::Job(int id, int dueDate, int readyDate, int penalty, unordered_map<Resource*, int> resources)
{
	this->id = id;
	this->dueDate = dueDate;
	this->readyDate = readyDate;
	this->penalty = penalty;
	this->resources = resources;
}

Job::~Job()
{
}

int Job::getId() const
{
	return this->id;
}

int Job::getStart()
{
	return this->start;
}

int Job::getEnd()
{
	return this->end;
}

int Job::getReadyDate() const
{
	return this->readyDate;
}

int Job::getDueDate() const
{
	return this->dueDate;
}

int Job::getResourceCount() const
{
	int counter = 0;
	for (pair<Resource*, int> pair : this->resources) {
		counter += pair.second;
	}
	return counter;
}

unordered_map<Resource*, int> Job::getRequiredResources() const
{
	return this->resources;
}

Machine * Job::getMachine()
{
	return this->machine;
}

void Job::reset()
{
	this->machine = nullptr;
	this->start = NULL;
	this->end = NULL;
	this->resetIntervals();
}

void Job::resetIntervals()
{
	for (pair<Resource*, ResourceInterval*> pair : this->intervals)
	{
		pair.first->free(pair.second);
		delete pair.second;
	}
	this->intervals.clear();
}

void Job::setSchedule(int start, int processingTime)
{
	this->start = start;
	this->end = start + processingTime;
}

void Job::setMachine(Machine * machine)
{
	this->machine = machine;
}

int Job::getCost()
{
	return max(0, this->end - this->dueDate)*this->penalty;
}

string Job::toString()
{
	return "Job#" + to_string(this->id);
}

void Job::addInterval(Resource * resource, ResourceInterval * interval)
{
	this->intervals[resource] = interval;
}

bool Job::startBefore(const Job * job1, const Job * job2)
{
	return job1->start < job2->start;
}

bool Job::dueDateBefore(const Job * job1, const Job * job2)
{
	return job1->dueDate < job2->dueDate;
}

bool Job::readyDateBefore(const Job * job1, const Job * job2)
{
	return job1->readyDate < job2->readyDate;
}

bool Job::moreUsedResourceBefore(const Job * job1, const Job * job2)
{
	return job1->getResourceCount() > job2->getResourceCount();
}

bool Job::minorMachineIdBefore(const Job * job1, const Job * job2)
{
	return job1->machine->getId() < job2->machine->getId();
}
