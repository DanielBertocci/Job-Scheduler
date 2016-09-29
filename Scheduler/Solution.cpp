#include "Solution.h"
#include <Windows.h>
#include "TemplateResource.h"

bool Solution::saveBest()
{
	int cost = this->calcCost();
	if (cost >= this->bestCost) {
		return false;
	}

	this->bestCost = cost;
	for (Job* job : this->jobs) {
		this->best[job][0] = job->getStart();
		this->best[job][1] = job->getEnd();
		this->best[job][2] = job->getMachine()->getId();
	}

	for (Machine* machine : this->machines) {
		this->savedSchedule[machine] = machine->getSchedule();
	}

	return true;
}

void Solution::loadBest()
{
	for (Resource* resource : this->resources) {
		resource->release();
	}
	for (Job* job : this->jobs)
	{
		job->setSchedule(this->best[job][0], this->best[job][1] - this->best[job][0]);
		job->setMachine(this->getMachineById(this->best[job][2]));
		for (pair<Resource*, int> pair : job->getRequiredResources()) {
			InstantIteratorSetPair instants = pair.first->use(this->best[job][0], this->best[job][1] - this->best[job][0], pair.second);
			(*instants.first)->setJob(job->getId());
			(*instants.second)->setJob(job->getId());
			job->setInstants(pair.first, instants);
		}
	}
	for (Machine* machine : this->machines) {
		machine->setSchedule(this->savedSchedule[machine]);
	}
}

Solution::Solution(DataContainer * data)
{
	this->data = data;

	// Build Resources objects.
	for (int i = 0; i < this->data->getResourceNumber(); ++i) {
		this->resources.push_back(
			new Resource(i, this->data->getResourceQuantity(i))
		);
	}

	//// Build Job objects.
	for (int i = 0; i < this->data->getJobNumber(); ++i) {
		int jobId = i + 1;
		unordered_map<Resource*, int> resources;

		for (Resource* resource : this->resources)
		{
			int required = this->data->getResourceRequired(jobId, resource->getId());
			if (required > 0) {
				resources[resource] = required;
			}
		}

		Job* job = new Job(
			jobId,
			this->data->getDueDate(jobId),
			this->data->getReleaseDate(jobId),
			this->data->getTardinessPenalty(jobId),
			resources
		);

		this->jobs.push_back(job);
	}

	// Build Machine objects.
	for (int i = 0; i < this->data->getMachineNumber(); ++i) {
		unordered_map<Job*, int> processingTime;
		unordered_map<Job*, unordered_map<Job*, int>> setupTime;

		// Build the processing time table for current machine.
		for (Job* job : this->jobs)
		{
			processingTime[job] = this->data->getProcessingTime(job->getId(), i);
		}

		// Build the setup time table for current machine.
		for (Job* firstJob : this->jobs)
		{
			for (Job* secondJob : this->jobs)
			{
				setupTime[firstJob][secondJob] = this->data->getSetupTime(i, firstJob->getId(), secondJob->getId());
			}
		}

		this->machines.push_back(
			new Machine(i, processingTime, setupTime)
		);
	}
}

Solution::~Solution()
{
}

Machine* Solution::getMachineById(int id)
{
	for (Machine* machine : this->machines) {
		if (machine->getId() == id) {
			return machine;
		}
	}
	return nullptr;
}

int Solution::getJobCount()
{
	return this->jobs.size();
}

int Solution::getSavedCost()
{
	return this->savedCost;
}

int Solution::calcCost()
{
	int cost = 0;
	/*for (Machine* machine : this->machines)
	{
		cost += machine->getCost();
	}*/
	for (Job* job : this->jobs)
	{
		cost += job->getCost();
	}
	this->cost = cost;
	return cost;
}

void Solution::improveResources()
{
}

void Solution::randomSchedule()
{
	srand(time(0));
	random_shuffle(this->jobs.begin(), this->jobs.end());

	this->reset();

	// Equi-distribution
	int i = 0;
	for (Job* job : this->jobs)
	{
		do {
			++i;
			i %= this->machines.size();
		} while (!this->machines[i]->addJob(job));
	}
}
void Solution::smartRandomSchedule()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::dueDateBefore);

	Machine* best;
	int processingTime;
	int counter = 0;
	for (Job* job : this->jobs) {
		best = nullptr;
		processingTime = INT_MAX;

		for (Machine* m : this->machines) {
			int tProcessingTime = m->getJobProcessingTime(job);

			if (tProcessingTime <= 0) continue;
			if (tProcessingTime < processingTime) {
				processingTime = tProcessingTime;
				best = m;
			}
		}
		counter++;
		best->addJob(job);
	}
}

void Solution::removeIdleFromMachines()
{
	random_shuffle(machines.begin(), machines.end());
	list<Machine*> machines(this->machines.begin(), this->machines.end());
	unordered_map<Machine*, JobListIterator> iterators;

	for (Machine* m : machines) {
		iterators[m] = m->tryRemoveIdle();
	}

	while (machines.size() != 0) {
		for (Machine* m : machines) {
			JobListIterator i = m->tryRemoveIdle(iterators[m]);
			if (i == iterators[m]) {
				machines.remove(m);
			}
			else {
				iterators[m] = i;
			}
		}
	}
}

void Solution::removeIdlesFromBest()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::startBefore);
	Job* prevJob = this->jobs.front();
	Job* currentLastJob = prevJob;
	unordered_map<int, Job*>lastMachineJob;
	int distance;
	int jobStart, jobEnd, jobMachine;
	int lastJobStart, lastJobEnd, lastJobMachine;
	int prevOnMachineStart, prevOnMachineEnd, setupOnMachine;
	for (Job* job : this->jobs) {
		jobStart = this->best[job][0];
		jobEnd = this->best[job][1];
		jobMachine = this->best[job][2];
		lastJobStart = this->best[currentLastJob][0];
		lastJobEnd = this->best[currentLastJob][1];
		lastJobMachine = this->best[currentLastJob][2];
		if (lastMachineJob[jobMachine] == nullptr) {
			prevOnMachineStart = 0;
			prevOnMachineEnd = 0;
			setupOnMachine = 0;
		}
		else {
			prevOnMachineStart = this->best[lastMachineJob[jobMachine]][0];
			prevOnMachineEnd = this->best[lastMachineJob[jobMachine]][1];
			setupOnMachine = this->data->getSetupTime(jobMachine, lastMachineJob[jobMachine]->getId(), job->getId());
		}
		if (prevJob->getEnd() > currentLastJob->getEnd()) {
			currentLastJob = prevJob;
		}

		lastMachineJob[this->best[prevJob][2]] = prevJob;

		distance = jobStart - lastJobEnd;
		if (lastMachineJob[jobMachine]) {
			distance = min(distance, jobStart - prevOnMachineEnd - setupOnMachine);
		}

		if (job->getStart() - distance < job->getReadyDate()) {
			distance = job->getStart() - job->getReadyDate();
		}

		if (distance > 0) {
			this->best[job][0] -= distance;
			this->best[job][1] -= distance;
		}
		prevJob = job;
	}
}

void Solution::schedule()
{
	/*random_shuffle(this->machines.begin(), this->machines.end());*/
	for (Machine* machine : this->machines)
	{
		machine->schedule();
	}
}
void Solution::reset()
{
	for (Machine* machine : this->machines)
	{
		machine->reset();
	}
	for (Resource* resource : this->resources) {
		resource->release();
	}
}

bool Solution::relaxMachinesCosts()
{
	Machine* expensive = machines.front();
	Machine* cheap = machines.front();

	for (Machine* m : this->machines) {
		if (m->getCost() < cheap->getCost()) {
			cheap = m;
		}
		if (m->getCost() > expensive->getCost()) {
			expensive = m;
		}
	}

	if (cheap == expensive) return false;

	return expensive->sendFirstAvailableJobToMachine(cheap);
}
bool Solution::relaxMachinesTimes()
{
	Machine* expensive = machines.front();
	Machine* cheap = machines.front();

	for (Machine* m : this->machines) {
		if (m->getTime() < cheap->getTime()) {
			cheap = m;
		}
		if (m->getTime() > expensive->getTime()) {
			expensive = m;
		}
	}

	if (cheap == expensive) return false;

	return expensive->sendFirstAvailableJobToMachine(cheap);
}
void Solution::randomJobSwapOnMachine()
{
	for (Machine* m : this->machines) {
		m->randomJobSwap();
		m->bestScheduleForCurrentJobs();
	}
}
void Solution::randomJobToAnotherMachine()
{
	vector<int> machines;
	Job* job;
	int counter = 0;
	do {
		++counter;
		int index = RandomGenerator::getInstance().randomInt(0, this->getJobCount() - 1);
		job = this->jobs[index];
		machines = this->data->getAvailableMachinesByJob(job->getId());
	} while (machines.size() <= 1 && counter < 5);

	int machineIndex = RandomGenerator::getInstance().randomInt(0, machines.size() - 1);

	Machine* oldMachine = job->getMachine();
	int newMachineId = machines[machineIndex];
	if (oldMachine->getId() == newMachineId) return;

	Machine* newMachine = this->getMachineById(newMachineId);

	oldMachine->removeJob(job);
	newMachine->addJobFront(job);

	oldMachine->schedule();
	newMachine->schedule();

	newMachine->bestScheduleForCurrentJobs();
}
void Solution::randomJobSwapBetweenMachines()
{
	int index1 = RandomGenerator::getInstance().randomInt(0, this->machines.size() - 1);
	int index2 = RandomGenerator::getInstance().randomInt(0, this->machines.size() - 1);

	if (index1 == index2) {
		index2 = (index1 + 1) % this->machines.size();
	}

	this->machines[index1]->swapRandomJobToMachine(this->machines[index2]);
	this->machines[index1]->bestScheduleForCurrentJobs();
	this->machines[index2]->bestScheduleForCurrentJobs();
}
void Solution::print(ostream & out)
{
	int cost = this->calcCost();
	sort(this->jobs.begin(), this->jobs.end(), Job::startBefore);

	out << "TWT;" << cost << ";;" << endl;
	out << "JobId;MachineId;Start;Completion" << endl;

	for (Job* job : this->jobs)
	{
		out << job->getId() << ";";
		out << job->getMachine()->getId() << ";";
		out << job->getStart() << ";" << job->getEnd() << endl;
	}
}

void Solution::localSearch()
{
	random_shuffle(this->machines.begin(), this->machines.end());
	for (Machine* m : this->machines) {
		m->bestScheduleForCurrentJobs();
	}
}

void Solution::localSearchNoised()
{
	int cost;
	do {
		for (Machine* m : this->machines) {
			int cost = m->getCost();
			m->schedulingShuffle();
			m->bestScheduleForCurrentJobs();
		}
		cost = this->calcCost();
	} while (cost < this->cost);
}
void Solution::localSearchCompressionFix()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::startBefore);
	Job* prevJob = this->jobs.front();
	Job* currentLastJob = prevJob;
	unordered_map<Machine*, Job*>lastMachineJob;
	int distance;
	for (auto it = this->jobs.begin(); it != this->jobs.end(); ++it) {
		Job* job = *it;
		if (prevJob->getEnd() > currentLastJob->getEnd()) {
			currentLastJob = prevJob;
		}

		lastMachineJob[prevJob->getMachine()] = prevJob;

		distance = job->getStart() - currentLastJob->getEnd();
		if (lastMachineJob[job->getMachine()]) {
			distance = min(distance, job->getStart() - lastMachineJob[job->getMachine()]->getEnd() - job->getMachine()->getSetupTime(lastMachineJob[job->getMachine()], job));
		}

		if (job->getStart() - distance < job->getReadyDate()) {
			distance = job->getStart() - job->getReadyDate();
		}

		if (distance > 0) {
			job->shiftLeft(distance);
		}
		prevJob = job;
	}
}

void Solution::printGraph()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::minorMachineIdBefore);
	stringstream ss;

	for (Job* job : this->jobs)
	{
		ss << "[";
		ss << "'Machine " << job->getMachine()->getId() << "',";
		ss << "'','";
		ss << "<div style = \"padding:5px;\">";
		ss << "<h3 style=\"border: 1px solid #000; margin-top: 0; padding: 5px;\">Job " << job->getId() << "</h3>";
		ss << "<b>Cost:</b> " << job->getCost() << "<br>";
		ss << "<b>Start : </b> " << job->getStart() << "<br>";
		ss << "<b>End : </b> " << job->getEnd() << "<br>";
		ss << "<b>Due Date : </b> " << job->getDueDate() << "<br>";
		ss << "<b>Ready Date : </b> " << job->getReadyDate() << "<br>";
		ss << "<b>Penalty:</b> " << job->getPenalty() << "<br>";
		ss << "<b>Resources:</b> ";
		for (auto p : job->getRequiredResources()) {
			ss << "(<b>" << p.first->getId() << "</b>," << p.second << ") ";
		}
		ss << "</div>";
		ss << "',";
		ss << "new Date(0,0,0,0," << job->getStart() << "),";
		ss << "new Date(0,0,0,0," << job->getEnd() << "),";
		ss << "]," << endl;
	}
	this->graph("solution_graph.html", ss.str());
}

void Solution::printResourceSchedulingGraph()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::minorMachineIdBefore);
	stringstream ss;
	auto a = this->resources;
	for (Resource* resource : this->resources)
	{
		for (Instant* i : resource->getUsage())
		{
			if (i->getQuantity() < 0) {
				continue;
			}

			ss << "[";
			ss << "'Resource#" << resource->getId() << "',";
			ss << "'','";
			ss << "<div style = \"padding:5px;\">";
			ss << "<h3 style=\"border: 1px solid #000; margin-top: 0; padding: 5px;\">Quantity " << i->getQuantity() << "</h3><b>Start:</b> " << i->getTime() << "<br><b>End:</b> " << i->next()->getTime() << "<br><b>Job:</b> " << i->getJob();
			ss << "</div>";
			ss << "',";
			ss << "new Date(0,0,0,0," << i->getTime() << "),";
			ss << "new Date(0,0,0,0," << i->next()->getTime() << "),";
			ss << "]," << endl;
		}
	}
	this->graph("solution_resource_graph.html", ss.str());
}

void Solution::printResourceUsageGraph()
{
	for (Resource* r : this->resources)
	{
		r->printUsage();
	}
}

void Solution::graph(string filePath, string jsData)
{
	HRSRC graphTemplateResource = FindResource(NULL, MAKEINTRESOURCE(IDR_HTML_TEMPLATE), RT_HTML);
	HGLOBAL graphTemplateResourceData = LoadResource(NULL, graphTemplateResource);
	string graphTemplate((char*)LockResource(graphTemplateResourceData));


	auto pos = graphTemplate.find("$DATA");
	if (pos != string::npos) {
		graphTemplate.replace(pos, 5, jsData);
	}


	ofstream out(filePath, ios::out | ios::binary);
	out << graphTemplate;
	out.close();
}

void Solution::graphArea(string filePath, string jsData)
{
	HRSRC graphTemplateResource = FindResource(NULL, MAKEINTRESOURCE(IDR_HTML_AREA_CHART), RT_HTML);
	HGLOBAL graphTemplateResourceData = LoadResource(NULL, graphTemplateResource);
	string graphTemplate((char*)LockResource(graphTemplateResourceData));


	auto pos = graphTemplate.find("$DATA");
	if (pos != string::npos) {
		graphTemplate.replace(pos, 5, jsData);
	}


	ofstream out(filePath, ios::out | ios::binary);
	out << graphTemplate;
	out.close();
}

void Solution::save()
{
	int i = 0;
	for (Job* job : this->jobs)
	{
		this->saved[job][0] = job->getStart();
		this->saved[job][1] = job->getEnd();
	}
	for (Machine* machine : this->machines) {
		this->savedSchedule[machine] = machine->getSchedule();
	}

	this->savedCost = this->calcCost();
	this->savedFlag = true;
}

void Solution::load()
{
	if (this->savedFlag == false) {
		return;
	}
	for (Resource* resource : this->resources) {
		resource->release();
	}
	for (Job* job : this->jobs)
	{
		job->setSchedule(this->saved[job][0], this->saved[job][1] - this->saved[job][0]);
		for (pair<Resource*, int> pair : job->getRequiredResources()) {
			InstantIteratorSetPair instants = pair.first->use(this->saved[job][0], this->saved[job][1] - this->saved[job][0], pair.second);
			(*instants.first)->setJob(job->getId());
			(*instants.second)->setJob(job->getId());
			job->setInstants(pair.first, instants);
		}
	}
	for (Machine* machine : this->machines) {
		machine->setSchedule(this->savedSchedule[machine]);
		for (Job* job : this->savedSchedule[machine]) {
			job->setMachine(machine);
		}
	}
}

int Solution::getTempCost()
{
	return this->tempSavedCost;
}

int Solution::getBestCost()
{
	return this->bestCost;
}

bool getRowUtil(istream &input, vector<string> &row) {
	string line, temp;

	if (!getline(input, line) || line == "" || line == "\r") return false;
	stringstream str(line);

	row.clear();
	while (getline(str, temp, ';'))
		row.push_back(temp);

	return true;
}

void Solution::store()
{
	string file = this->data->getSolutionFile();

	/*ifstream f(file);
	if (f.good()) {
		vector<string>line;
		getRowUtil(f, line);
		if (atoi(line[1].c_str()) <= this->calcCost()) {
			return;
		}
	}*/

	ofstream out(file);
	this->print(out);
	out.close();
}

void Solution::storeBest()
{
	string file = this->data->getSolutionFile();
	ofstream out(file);
	this->printBest(out);
	out.close();
}

void Solution::printBest(ofstream& out)
{
	for (Job* job : this->jobs) {
		job->setSchedule(this->best[job][0], this->best[job][1] - this->best[job][0]);
	}

	sort(this->jobs.begin(), this->jobs.end(), Job::startBefore);

	out << "TWT;" << this->bestCost << ";;" << endl;
	out << "JobId;MachineId;Start;Completion" << endl;

	for (Job* job : this->jobs)
	{
		int* a = this->best[job];
		out << job->getId() << ";";
		out << this->best[job][2] << ";"; // MachineID.
		out << this->best[job][0] << ";" << this->best[job][1] << endl; // Start & End.
	}
}
