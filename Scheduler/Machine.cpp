#include "Machine.h"
Machine::Machine(int id, JobProcessingTimeMap processingTime, JobSetupTimeMap setupTime)
{
	this->id = id;
	this->processingTime = processingTime;
	this->setupTime = setupTime;
}

Machine::~Machine()
{
}

int Machine::getId()
{
	return this->id;
}

int Machine::getCost()
{
	return this->cost;
}

int Machine::getTime()
{
	return this->time;
}

int Machine::getSetupTime(Job * prev, Job * next)
{
	return this->setupTime[prev][next];
}

JobList Machine::getSchedule()
{
	return this->scheduledJobs;
}

int Machine::getJobProcessingTime(Job * job)
{
	return this->processingTime[job];
}

int Machine::getStartForAllResources(Job * job, int start)
{
	int searchStart = start;

	for (pair<Resource*, int> r : job->getRequiredResources()) {
		searchStart = max(searchStart, r.first->getFirstFreeInstant(searchStart, this->processingTime[job], r.second));
	}
	if (searchStart > start) {
		return this->getStartForAllResources(job, searchStart);
	}
	else {
		return searchStart;
	}
}

void Machine::resetJobResourcesFrom(JobListIterator iterator)
{
	while (iterator != this->scheduledJobs.end()) {
		(*iterator)->resetInstants();
		++iterator;
	}
}

JobList Machine::getJobSchedulableOnMachine(Machine * machine)
{
	JobList schedulableOnOtherMachine;

	for (JobListIterator iterator = this->scheduledJobs.begin(); iterator != this->scheduledJobs.end(); ++iterator) {
		if (machine->processingTime[*iterator] > 0) {
			schedulableOnOtherMachine.push_back(*iterator);
		}
	}

	return schedulableOnOtherMachine;
}
void Machine::calcCostTo(JobListIterator iterator)
{
	this->cost = 0;
	auto current = this->scheduledJobs.begin();
	while (current != iterator) {
		this->cost += (*current)->getCost();
		current = next(current);
	}
}

void Machine::scheduleFrom(JobListIterator iterator)
{
	Job* previousJob;

	// Get cost and release other resources.
	this->calcCostTo(iterator);
	this->resetJobResourcesFrom(iterator);

	// Set machine time and previous job.
	if (iterator == this->scheduledJobs.begin()) {
		this->time = 0;
		previousJob = nullptr;
	}
	else {
		previousJob = *prev(iterator);
		this->time = previousJob->getEnd();
	}

	while (iterator != this->scheduledJobs.end()) {
		Job* job = *iterator;

		// Cycle counter;
		++iterator;

		this->time += this->setupTime[previousJob][job];
		previousJob = job;

		// Wait execution until job ready.
		if (this->time < job->getReadyDate()) {
			this->time = job->getReadyDate();
		}

		// Search first interval with all resources required.
		if (job->getRequiredResources().size() == 1) {
			for (pair<Resource*, int> r : job->getRequiredResources()) {
				InstantIteratorSetPair i = r.first->useFirstFreeSlot(this->time, this->processingTime[job], r.second);
				(*i.first)->setJob(job->getId());
				(*i.second)->setJob(job->getId());
				job->setInstants(r.first, i);
				this->time = (*i.first)->getTime();
			}
		}
		else {
			int firstStartAvailable = this->getStartForAllResources(job, this->time);
			InstantIteratorSetPair check;
			for (pair<Resource*, int> r : job->getRequiredResources()) {
				check = r.first->use(firstStartAvailable, this->processingTime[job], r.second);
				job->setInstants(r.first, check);
				(*check.first)->setJob(job->getId());
				(*check.second)->setJob(job->getId());
			}
			this->time = firstStartAvailable;
		}

		// Set start end in the job object.
		job->setSchedule(this->time, this->processingTime[job]);
		this->cost += job->getCost();

		// Wait the job processing time.
		this->time = job->getEnd();
	}

	this->scheduled = true;
}
void Machine::setSchedule(JobList schedule)
{
	this->scheduledJobs = schedule;
}

bool Machine::addJob(Job * job)
{
	int processingTime = this->processingTime[job];

	// The job cannot be processed on this machine.
	if (processingTime <= 0) return false;

	// Set pointers.
	this->scheduledJobs.push_back(job);
	job->setMachine(this);

	return true;
}

void Machine::schedule()
{
	this->scheduleFrom(this->scheduledJobs.begin());
}

void Machine::resetJobResources()
{
	this->resetJobResourcesFrom(this->scheduledJobs.begin());
}

void Machine::reset()
{
	for (Job* job : this->scheduledJobs)
	{
		job->reset();
	}
	this->scheduledJobs.clear();
}

void Machine::schedulingShuffle()
{
	int randomIndex;
	for (auto it = this->scheduledJobs.begin(); it != this->scheduledJobs.end(); ++it) {
		randomIndex = RandomGenerator::getInstance().randomInt(0, this->scheduledJobs.size() - 1);
		iter_swap(it, next(this->scheduledJobs.begin(), randomIndex));
	}

	this->schedule();
}

void Machine::randomJobSwap()
{
	int i = RandomGenerator::getInstance().randomInt(0, this->scheduledJobs.size() - 1);
	int j = RandomGenerator::getInstance().randomInt(0, this->scheduledJobs.size() - 1);

	if (i == j) return;

	auto first = next(this->scheduledJobs.begin(), i);
	auto second = next(this->scheduledJobs.begin(), j);

	iter_swap(first, second);
}

void Machine::improveTryAllSwap()
{
	int best = this->cost;
	int skipFix = 0;

	auto current = this->scheduledJobs.begin();
	while (current != prev(this->scheduledJobs.end()) && best > 0) {
		iter_swap(current, next(current));
		this->scheduleFrom(prev(current, skipFix));

		if (this->cost > best) {
			iter_swap(next(current), current);
			++skipFix;
		}
		else {
			best = this->cost;
			skipFix = max(0, skipFix - 1);
		}
		++current;
	}

	// Fix last swap.
	this->scheduleFrom(prev(this->scheduledJobs.end()));
}


bool Machine::swapRandomJobToMachine(Machine * machine)
{
	// Gets a list of schedulable jobs.
	JobList schedulableOnThisMachine = machine->getJobSchedulableOnMachine(this);
	JobList schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

	if (schedulableOnThisMachine.size() <= 0 || schedulableOnOtherMachine.size() <= 0) {
		return false;
	}

	int index1 = RandomGenerator::getInstance().randomInt(0, schedulableOnOtherMachine.size() - 1);
	int index2 = RandomGenerator::getInstance().randomInt(0, schedulableOnThisMachine.size() - 1);

	// Job* iterators.
	Job* j1 = *next(schedulableOnOtherMachine.begin(), index1);
	Job* j2 = *next(schedulableOnThisMachine.begin(), index2);

	// Schedule job.
	this->addJob(j2);
	machine->addJob(j1);

	// Remove from schedule vector.
	auto fromThis = find(this->scheduledJobs.begin(), this->scheduledJobs.end(), j1);
	if (fromThis != this->scheduledJobs.begin()) {
		fromThis = prev(fromThis);
		this->scheduledJobs.remove(j1);
		this->scheduleFrom(fromThis);
	}
	else {
		this->scheduledJobs.remove(j1);
		this->schedule();
	}

	auto fromOther = find(machine->scheduledJobs.begin(), machine->scheduledJobs.end(), j2);
	if (fromOther != machine->scheduledJobs.begin()) {
		fromOther = prev(fromOther);
		machine->scheduledJobs.remove(j2);
		machine->scheduleFrom(fromOther);
	}
	else {
		machine->scheduledJobs.remove(j2);
		machine->schedule();
	}

	return true;
}

bool Machine::sendFirstAvailableJobToMachine(Machine * machine)
{
	JobList schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

	if (schedulableOnOtherMachine.size() <= 0) {
		return false;
	}

	int randomIndex = RandomGenerator::getInstance().randomInt(0, schedulableOnOtherMachine.size() - 1);
	Job* job = *next(schedulableOnOtherMachine.begin(), randomIndex);

	// Schedule on other machine.
	machine->addJob(job);
	this->scheduledJobs.remove(job);

	this->schedule();
	machine->schedule();

	return true;
}

void Machine::previousSchedule()
{
	if (this->previousScheduledJobs.empty()) {
		runtime_error("There is no previous scheduling");
	}
	this->scheduledJobs = this->previousScheduledJobs;
	this->cost = this->previousCost;
}

void Machine::bestScheduleForCurrentJobs()
{
	int cost;

	do {
		cost = this->cost;
		this->improveTryAllSwap();
	} while (this->cost < cost);
}
