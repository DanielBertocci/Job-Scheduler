#include "Machine.h"

void Machine::calcCostTo(list<Job*>::iterator iterator)
{
	this->cost = 0;
	auto current = this->scheduledJobs.begin();
	while (current != iterator) {
		this->cost += (*current)->getCost();
		++current;
	}
}

void Machine::scheduleFrom(list<Job*>::iterator iterator)
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
				ResourceInterval* i = r.first->useFirstFreeSlot(this->time, this->processingTime[job], r.second);
				job->addInterval(r.first, i);
				this->time = max(this->time, i->getStart());
			}
		}
		else {
			int firstStartAvailable = this->getStartForAllResources(job, this->time);
			ResourceInterval* check = nullptr;
			for (pair<Resource*, int> r : job->getRequiredResources()) {
				check = r.first->use(firstStartAvailable, this->processingTime[job], r.second);
				job->addInterval(r.first, check);
			}
			this->time = firstStartAvailable;
		}

		// Set start end in the job object.
		job->setSchedule(this->time, this->processingTime[job]);
		this->cost += job->getCost();

		// Wait the job processing time.
		this->time += this->processingTime[job];
	}

	this->scheduled = true;
}

void Machine::resetJobResourcesFrom(list<Job*>::iterator iterator)
{
	while (iterator != this->scheduledJobs.end()) {
		(*iterator)->resetIntervals();
		++iterator;
	}
}

list<Job*> Machine::getJobSchedulableOnMachine(Machine * machine)
{
	list<Job*> schedulableOnOtherMachine;

	for (list<Job*>::iterator iterator = this->scheduledJobs.begin(); iterator != this->scheduledJobs.end(); ++iterator) {
		if (machine->processingTime[*iterator] > 0) {
			schedulableOnOtherMachine.push_back(*iterator);
		}
	}

	return schedulableOnOtherMachine;
}

int Machine::getStartForAllResources(Job * job, int start)
{
	int searchStart = start;

	for (pair<Resource*, int> r : job->getRequiredResources()) {
		searchStart = max(searchStart, r.first->getFirstFreeSlot(searchStart, this->processingTime[job], r.second)->getStart());
	}
	if (searchStart > start) {
		return this->getStartForAllResources(job, searchStart);
	}
	else {
		return searchStart;
	}
}

Machine::Machine(int id, unordered_map<Job*, int> processingTime, unordered_map<Job*, unordered_map<Job*, int>> setupTime)
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

Job * Machine::getLastScheduledJob()
{
	if (this->scheduledJobs.size() <= 0) {
		return nullptr;
	}

	return this->scheduledJobs.back();
}

void Machine::printSchedule()
{
	for (Job* job : this->scheduledJobs)
	{
		cout << job->toString() << " ";
	}
	cout << endl;
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

void Machine::shiftScheduledByDueDate()
{
	this->previousScheduledJobs = this->scheduledJobs;
	list<Job*>::iterator end = this->scheduledJobs.end();
	for (list<Job*>::iterator current = this->scheduledJobs.begin(); current != prev(end); ++current) {
		list<Job*>::iterator succeding = next(current);
		if (Job::dueDateBefore(*current, *succeding)) {
			iter_swap(current, succeding);
		}
	}
}

void Machine::randomJobSwap()
{
	this->storeCurrentScheduling();

	int i = rand() % (this->scheduledJobs.size() - 1);
	auto first = next(this->scheduledJobs.begin(), i);

	if (first == this->scheduledJobs.end()) {
		first = prev(first);
	}

	auto second = next(first);

	iter_swap(first, second);
}

void Machine::improveSetup()
{
	this->storeCurrentScheduling();

	auto current = this->scheduledJobs.begin();
	while (current != prev(this->scheduledJobs.end())) {
		auto succeding = next(current);
		if (this->setupTime[*current][*succeding] > this->setupTime[*succeding][*current]) {
			iter_swap(current, succeding);
		}
		++current;
	}
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

void Machine::storeCurrentScheduling()
{
	this->previousScheduledJobs = this->scheduledJobs;
}

bool Machine::swapRandomJobToMachine(Machine * machine)
{
	// Gets a list of schedulable jobs.
	list<Job*> schedulableOnThisMachine = machine->getJobSchedulableOnMachine(this);
	list<Job*> schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

	if (schedulableOnThisMachine.size() <= 0 || schedulableOnOtherMachine.size() <= 0) {
		return false;
	}

	// Job* iterators.
	Job* j1 = *next(schedulableOnOtherMachine.begin(), rand() % schedulableOnOtherMachine.size());
	Job* j2 = *next(schedulableOnThisMachine.begin(), rand() % schedulableOnThisMachine.size());

	// Schedule job.
	this->addJob(j2);
	machine->addJob(j1);

	// Remove from schedule vector.
	this->scheduledJobs.remove(j1);
	machine->scheduledJobs.remove(j2);

	// Schedule new job.
	this->schedule();
	machine->schedule();

	return true;
}

bool Machine::sendFirstAvailableJobToMachine(Machine * machine)
{
	list<Job*> schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

	if (schedulableOnOtherMachine.size() <= 0) {
		return false;
	}

	Job* job = *next(schedulableOnOtherMachine.begin(), rand() % schedulableOnOtherMachine.size());

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
	this->previousScheduledJobs.clear();
}
