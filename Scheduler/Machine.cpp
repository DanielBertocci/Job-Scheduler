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

void Machine::swapJobs(JobListIterator j1, JobListIterator j2)
{
	Job* a = *j1;
	Job* b = *j2;

	int newAStart = a->getReadyDate();
	int newBStart = b->getReadyDate();
	if (j1 != this->scheduledJobs.begin()) {
		newBStart = max((*prev(j1))->getEnd() + this->setupTime[*prev(j1)][*j2], newBStart);
	}
	b->setSchedule(newBStart, b->getEnd() - b->getStart());
	newAStart = max(newAStart, b->getEnd() + this->setupTime[b][a]);
	a->setSchedule(newAStart, a->getEnd() - a->getStart());
}

bool Machine::canSwapImproveCost(JobListIterator j1, JobListIterator j2)
{
	Job* a = *j1;
	Job* b = *j2;
	int oldCost = a->getCost() + b->getCost();
	int oldAStart = a->getStart();
	int oldAEnd = a->getEnd();
	int oldBStart = b->getStart();
	int oldBEnd = b->getEnd();
	int oldEndTime = b->getEnd();
	int newEndTime = 0;
	int newAStart, newAEnd, newBStart, newBEnd, newCost;

	// B Job.
	newBStart = oldAStart;
	if (j1 != this->scheduledJobs.begin()) {
		newBStart += (this->setupTime[*prev(j1)][*j2] - this->setupTime[*prev(j1)][*j1]);
	}

	newBStart = max(newBStart, b->getReadyDate());
	newBEnd = newBStart + b->getEnd() - b->getStart();

	// A Job.
	newAStart = newBEnd + this->setupTime[*j2][*j1];
	newAStart = max(newAStart, a->getReadyDate());
	newAEnd = newAStart + (oldAEnd - oldAStart);

	// End times.
	if (j2 != prev(this->scheduledJobs.end())) {
		oldEndTime = (*next(j2))->getStart();
		newEndTime = this->setupTime[*j1][*next(j2)];
	}
	newEndTime += newAEnd;

	// Cost.
	newCost = max(newAEnd - a->getDueDate(), 0)*a->getPenalty() + max(newBEnd - b->getDueDate(), 0)*b->getPenalty();

	//// Case LAST: j2 is tha last scheduled job.
	//if (j2 == prev(this->scheduledJobs.end())) {
	//	return newCost < oldCost;
	//}

	return newEndTime <= oldEndTime;
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
	if (iterator == this->scheduledJobs.end()) {
		return;
	}

	Job* previousJob;
	Job* firstScheduledJob = *iterator;
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
		job->schedulingMachinePosition = iterator;
		if (job->schedulingPosition != Job::scheduling.end()) {
			Job::scheduling.erase(job->schedulingPosition);
		}

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
		job->schedulingPosition = Job::scheduling.insert(job);
		this->cost += job->getCost();

		// Wait the job processing time.
		this->time = job->getEnd();
	}

	auto it = firstScheduledJob->schedulingPosition;
	/*while (it != Job::scheduling.end() && (*it)->getMachine() == this)
	{
		++it;
	}
	if (it == Job::scheduling.end()) {
		return;
	}
	(*it)->getMachine()->scheduleFrom((*it)->schedulingMachinePosition);*/

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

bool Machine::addJobFront(Job * job)
{
	int processingTime = this->processingTime[job];

	// The job cannot be processed on this machine.
	if (processingTime <= 0) return false;

	// Set pointers.
	this->scheduledJobs.push_front(job);
	job->setMachine(this);

	return true;
}

JobListIterator Machine::removeJob(Job * job)
{
	JobListIterator find = this->scheduledJobs.end();
	for (JobListIterator i = this->scheduledJobs.begin(); i != this->scheduledJobs.end(); ++i) {
		if ((*i)->getId() == job->getId()) {
			find = i;
			break;
		}
	}

	if (find == this->scheduledJobs.end()) {
		runtime_error("The job is not scheduled");
	}

	return this->scheduledJobs.erase(find);
}

JobListIterator Machine::removeJob(JobListIterator job)
{
	return this->scheduledJobs.erase(job);
}

void Machine::schedule()
{
	this->scheduleFrom(this->scheduledJobs.begin());
}

void Machine::saveCost()
{
	this->savedCost = this->cost;
}

void Machine::loadCost()
{
	this->cost = this->savedCost;
}

JobListIterator Machine::tryRemoveIdle()
{
	return this->tryRemoveIdle(this->scheduledJobs.begin());
}

JobListIterator Machine::tryRemoveIdle(JobListIterator iterator)
{
	if (iterator == this->scheduledJobs.end()) {
		return iterator;
	}

	JobListIterator endSearch = prev(this->scheduledJobs.end(), 2);
	Job* current;
	Job* nextJob;
	int span;
	int oldCost = this->cost;
	JobListIterator i;
	for (i = iterator; next(i) != endSearch; ++i) {
		current = *i;
		nextJob = *next(i);
		span = nextJob->getStart() - current->getEnd() - this->setupTime[current][nextJob];
		if (span > 0) {
			this->scheduleFrom(i);
			if (this->cost < oldCost) {
				return next(i);
			}
		}
	}
	return this->scheduledJobs.end();
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

Machine* Machine::sendJobWithSmallerProcessingTimeOnAnotherMachine()
{
	Job* job = nullptr;
	Job* current = nullptr;

	for (auto i = this->scheduledJobs.rbegin(); i != this->scheduledJobs.rend(); ++i) {
		current = *i;
		if (current->getBestMachine() != this) {
			job = current;
			break;
		}
	}

	if (job == nullptr) return nullptr;
	job->sendToMachine(job->getBestMachine());

	return job->getBestMachine();
}

bool Machine::expensiveBefore(const Machine * m1, const Machine * m2)
{
	return m1->cost > m2->cost;
}

void Machine::randomJobSwap()
{
	int i = RandomGenerator::getInstance().randomInt(0, this->scheduledJobs.size() - 1);
	int j = RandomGenerator::getInstance().randomInt(0, this->scheduledJobs.size() - 1);

	if (i == j) return;

	auto first = next(this->scheduledJobs.begin(), i);
	auto second = next(this->scheduledJobs.begin(), j);

	iter_swap(first, second);
	if (i <= j) {
		this->scheduleFrom(first);
	}
	else {
		this->scheduleFrom(second);
	}
}

void Machine::improveTryAllSwap()
{
	int best = this->cost;
	int skipFix = 0;

	auto current = this->scheduledJobs.begin();
	while (current != prev(this->scheduledJobs.end()) && best > 0) {
		/*if (!this->canSwapImproveCost(current, next(current))) {
			++current;
			continue;
		}*/
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
	this->addJobFront(j2);
	machine->addJobFront(j1);

	this->scheduledJobs.remove(j1);
	machine->scheduledJobs.remove(j2);

	this->schedule();
	machine->schedule();

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
	machine->addJobFront(job);
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
