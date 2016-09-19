#include "Solution.h"
#include <Windows.h>
#include "TemplateResource.h"

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

		this->jobs.push_back(
			new Job(
				jobId,
				this->data->getDueDate(jobId),
				this->data->getReleaseDate(jobId),
				this->data->getTardinessPenalty(jobId),
				resources
			)
		);
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

int Solution::jobCount()
{
	return this->jobs.size();
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
void Solution::dueDateBasedSchedule()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::dueDateBefore);
	int i = 0;
	for (Job* job : this->jobs)
	{
		do {
			++i;
			i %= this->machines.size();
		} while (!this->machines[i]->addJob(job));
	}
}
void Solution::readyDateBasedSchedule()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::readyDateBefore);
	int i = 0;
	for (Job* job : this->jobs)
	{
		do {
			++i;
			i %= this->machines.size();
		} while (!this->machines[i]->addJob(job));
	}
}
void Solution::resourceUsedBasedSchedule()
{
	sort(this->jobs.begin(), this->jobs.end(), Job::moreUsedResourceBefore);
	int i = 0;
	for (Job* job : this->jobs)
	{
		do {
			++i;
			i %= this->machines.size();
		} while (!this->machines[i]->addJob(job));
	}
}
void Solution::schedule()
{
	random_shuffle(this->machines.begin(), this->machines.end());
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
void Solution::printSchedule()
{
	for (Machine* machine : this->machines)
	{
		cout << "Machine" << machine->getId() << ":";
		machine->printSchedule();
		cout << "\n";
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
	}
}
void Solution::randomJobSwapBetweenMachines()
{
	random_shuffle(this->machines.begin(), this->machines.end());

	auto current = this->machines.begin();

	(*current)->swapRandomJobToMachine(*(current + 1));
	(*current)->bestScheduleForCurrentJobs();
	(*(current + 1))->bestScheduleForCurrentJobs();
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
void Solution::swapJobsOnMachine()
{
	this->swapJobsOnMachine(1);
}
void Solution::partialShuffle()
{
	for (Machine* machine : this->machines) {
		machine->partialShuffle();
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
void Solution::tardinessFix()
{
	random_shuffle(this->machines.begin(), this->machines.end());
	for (Machine* m : this->machines) {
		m->expansiveJobReschedule();
	}
}
void Solution::swapJobsOnMachine(int iterations)
{
	random_shuffle(this->machines.begin(), this->machines.end());
	for (int i = 0; i < iterations; ++i) {
		for (Machine* machine : this->machines) {
			machine->improveTryAllSwap();
		}
	}
}

void Solution::setupTest()
{
	for (Machine* machine : this->machines) {
		int cost = machine->getCost();
		machine->improveSetup();
		machine->schedule();
		int newcost = machine->getCost();
		if (newcost > cost) {
			machine->previousSchedule();
		}
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
		ss << "<h3 style=\"border: 1px solid #000; margin-top: 0; padding: 5px;\">Job " << job->getId() << "</h3><b>Cost:</b> " << job->getCost() << "<br><b>Start:</b> " << job->getStart() << "<br><b>End:</b> " << job->getEnd() << "<br><b>Due Date:</b> " << job->getDueDate() << "<br><b>Ready Date:</b> " << job->getReadyDate();
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
			ss << "<h3 style=\"border: 1px solid #000; margin-top: 0; padding: 5px;\">Quantity " << i->getQuantity() << "</h3><b>Start:</b> " << i->getTime() << "<br><b>End:</b> " << i->next()->getTime();
			ss << "</div>";
			ss << "',";
			ss << "new Date(0,0,0,0," << i->getTime() << "),";
			ss << "new Date(0,0,0,0," << i->next()->getTime() << "),";
			ss << "]," << endl;
		}
	}
	this->graph("solution_resource_graph.html", ss.str());
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

void Solution::save()
{
	int i = 0;
	for (Job* job : this->jobs)
	{
		this->saved[job] = new int[3]{ job->getMachine()->getId(), job->getStart(), job->getEnd() };
	}
	for (Machine* machine : this->machines) {
		this->savedSchedule[machine] = machine->getSchedule();
	}
	for (Resource* resource : this->resources) {
		this->savedResources[resource] = resource->getUsage();
	}
	this->savedCost = this->calcCost();
	this->savedFlag = true;
}

void Solution::load()
{
	if (this->savedFlag == false) {
		return;
	}
	for (Job* job : this->jobs)
	{
		job->setMachine(this->getMachineById(this->saved[job][0]));
		job->setSchedule(this->saved[job][1], this->saved[job][2] - this->saved[job][1]);
	}
	for (Machine* machine : this->machines) {
		machine->setSchedule(this->savedSchedule[machine]);
	}
	for (Resource* resource : this->resources) {
		resource->setUsage(this->savedResources[resource]);
	}
}

void Solution::saveTemp()
{
	int i = 0;
	for (Job* job : this->jobs)
	{
		this->savedTemp[job] = new int[3]{ job->getMachine()->getId(), job->getStart(), job->getEnd() };
	}
	for (Machine* machine : this->machines) {
		this->savedScheduleTemp[machine] = machine->getSchedule();
	}
	this->tempSavedCost = this->calcCost();
}

void Solution::loadTemp()
{
	for (Job* job : this->jobs)
	{
		job->setMachine(this->getMachineById(this->savedTemp[job][0]));
		job->setSchedule(this->savedTemp[job][1], this->savedTemp[job][2] - this->savedTemp[job][1]);
	}
	for (Machine* machine : this->machines) {
		machine->setSchedule(this->savedScheduleTemp[machine]);
	}
}

int Solution::getTempCost()
{
	return this->tempSavedCost;
}

void Solution::store()
{
	string file = this->data->getSolutionFile();
	ofstream out(file);
	this->print(out);
	out.close();
}
