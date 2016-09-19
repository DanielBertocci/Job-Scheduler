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
	this->resetInstants();
}

void Job::resetInstants()
{
	for (pair<Resource*, pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator>> pair : this->instants)
	{
		pair.first->free(pair.second.first);
		pair.first->free(pair.second.second);
		/*delete *pair.second.first;
		delete *pair.second.second;*/
	}
	this->instants.clear();
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

void Job::shiftLeft(int amount)
{
	this->start -= amount;
	this->end -= amount;
}

string Job::toString()
{
	return "Job#" + to_string(this->id);
}

void Job::addInstant(Resource * resource, pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> interval)
{
	this->instants[resource] = interval;
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
