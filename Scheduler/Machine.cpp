#include "Machine.h"

void Machine::calcCostTo(list<Job*>::iterator iterator)
{
	this->cost = 0;
	auto current = this->scheduledJobs.begin();
	while (current != iterator) {
		this->cost += (*current)->getCost();
		current = next(current);
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
				pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> i = r.first->useFirstFreeSlot(this->time, this->processingTime[job], r.second);
				job->addInstant(r.first, i);
				this->time = (*i.first)->getTime();
			}
		}
		else {
			int firstStartAvailable = this->getStartForAllResources(job, this->time);
			pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> check;
			for (pair<Resource*, int> r : job->getRequiredResources()) {
				check = r.first->use(firstStartAvailable, this->processingTime[job], r.second);
				job->addInstant(r.first, check);
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
		(*iterator)->resetInstants();
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
		searchStart = max(searchStart, r.first->getFirstFreeInstant(searchStart, this->processingTime[job], r.second));
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

int Machine::getTime()
{
	return this->time;
}

int Machine::getJobProcessingTime(Job * job)
{
	return this->processingTime[job];
}

list<Job*> Machine::getSchedule()
{
	return this->scheduledJobs;
}

void Machine::setSchedule(list<Job*> schedule)
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

void Machine::partialShuffle()
{
	/*int begin = floor(this->scheduledJobs.size() / 4);
	int end = ceil((3 * this->scheduledJobs.size()) / 4);
	list<Job*>::iterator startShuffleIterator = next(this->scheduledJobs.begin(), begin);
	list<Job*>::iterator endShuffleIterator = next(this->scheduledJobs.begin(), end);
	int counter = 0;
	while (counter < end - begin) {
		iter_swap(startShuffleIterator);
	}
	this->scheduleFrom(startShuffleIterator);*/
}

void Machine::expansiveJobReschedule()
{
	// Get the most expansive job.
	list<Job*>::iterator exspansiveJobIterator = this->scheduledJobs.begin();
	list<Job*>::iterator current = this->scheduledJobs.begin();
	while (current != this->scheduledJobs.end()) {
		if ((*current)->getCost() > (*exspansiveJobIterator)->getCost()) {
			exspansiveJobIterator = current;
		}
		current = next(current);
	}

	int cost = this->cost;

	// One of the first three job schedule position.
	auto earlyJobIterator = next(this->scheduledJobs.begin(), rand() % 3);
	iter_swap(earlyJobIterator, exspansiveJobIterator);
	this->schedule();

	/*if (this->cost > cost) {
		iter_swap(exspansiveJobIterator, earlyJobIterator);
		this->scheduleFrom(earlyJobIterator);
		return;
	}*/

	cost = this->cost;

	do {
		iter_swap(earlyJobIterator, next(earlyJobIterator));
		this->scheduleFrom(earlyJobIterator);
		earlyJobIterator = next(earlyJobIterator);
		cost = this->cost;
	} while (this->cost < cost && earlyJobIterator != this->scheduledJobs.end());

	iter_swap(exspansiveJobIterator, prev(exspansiveJobIterator));
	this->scheduleFrom(earlyJobIterator);
	return;
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
	this->previousCost = this->cost;
}

bool Machine::swapRandomJobToMachine(Machine * machine)
{
	// Gets a list of schedulable jobs.
	list<Job*> schedulableOnThisMachine = machine->getJobSchedulableOnMachine(this);
	list<Job*> schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

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
	list<Job*> schedulableOnOtherMachine = this->getJobSchedulableOnMachine(machine);

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
