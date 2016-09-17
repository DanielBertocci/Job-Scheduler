#include "DataContainer.h"

// Utility to parse CSV.
bool getRow(istream &input, vector<string> &row) {
	string line, temp;

	if (!getline(input, line) || line == "" || line == "\r") return false;
	stringstream str(line);

	row.clear();
	while (getline(str, temp, ';'))
		row.push_back(temp);

	return true;
}

// Removes a line from a stream.
void removeLine(istream &input) {
	string line;
	getline(input, line);
}

// Initialize matrixes with -1.
int** matrixInitializer(int p, int q) {
	int** matrix = new int*[p];
	for (int i = 0; i < p; ++i)
		matrix[i] = new int[q];

	for (int i = 0; i < p; ++i)
		for (int j = 0; j < q; ++j)
			matrix[i][j] = -1;

	return matrix;
}

void DataContainer::parseJobsNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Jobs") {
		throw invalid_argument("File bad format: the first line is required to be the jobs number.");
	}

	this->jobs = stoi(row[1]);
}

void DataContainer::parseMachinesNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Machines") {
		throw invalid_argument("File bad format: the second line is required to be the machines number.");
	}

	this->machines = stoi(row[1]);
}

void DataContainer::parseResourcesNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Resources") {
		throw invalid_argument("File bad format: the third line is required to be the resources number.");
	}

	this->resources = stoi(row[1]);
}

void DataContainer::parseProcessingTime(istream & in)
{
	vector<string> row;
	int jobId, machineId, pTime;

	// Initialize the processingTime matrix.
	this->processingTime = matrixInitializer(this->jobs, this->machines);

	// Parse.
	while (getRow(in, row)) {
		jobId = stoi(row[0]);
		machineId = stoi(row[1]);
		pTime = stoi(row[2]);

		this->processingTime[jobId - 1][machineId] = pTime;
	}
}

void DataContainer::parseTimeTable(istream & in)
{
	vector<string> row;
	int jobId, readyTime, dueDate, tardinessPenalty;

	// Initialize the timeTable.
	this->timeTable = matrixInitializer(this->jobs, 3);

	// Parse.
	while (getRow(in, row)) {
		jobId = stoi(row[0]);
		dueDate = stoi(row[1]);
		readyTime = stoi(row[2]);
		tardinessPenalty = stoi(row[3]);

		timeTable[jobId - 1][0] = dueDate;
		timeTable[jobId - 1][1] = readyTime;
		timeTable[jobId - 1][2] = tardinessPenalty;
	}
}

void DataContainer::parseSetupTimeTable(istream & in)
{
	vector<string> row;
	int machineId, beforeJob, afterJob, setupTime;

	// Initialize the setup time table.
	this->setupTimeTable = new int**[this->machines];
	for (int i = 0; i < this->machines; ++i)
		this->setupTimeTable[i] = matrixInitializer(this->jobs, this->jobs);

	// Parse.
	while (getRow(in, row)) {
		machineId = stoi(row[0]);
		beforeJob = stoi(row[1]);
		afterJob = stoi(row[2]);
		setupTime = stoi(row[3]);

		this->setupTimeTable[machineId][beforeJob - 1][afterJob - 1] = setupTime;
	}
}

void DataContainer::parseResourceQuantity(istream & in)
{
	vector<string> row;
	int resourceId, quantity;

	// Initialize.
	this->resourceQuantity = new int[this->resources];

	//Parse.
	while (getRow(in, row)) {
		resourceId = std::stoi(row[0]);
		quantity = std::stoi(row[1]);
		this->resourceQuantity[resourceId] = quantity;
	}
}

void DataContainer::parseResourceRequired(istream & in)
{
	vector<string> row;
	int jobId, resourceId, rRequired;

	// Initialize.
	this->resourceRequired = matrixInitializer(this->jobs, this->resources);

	// Parse.
	while (getRow(in, row)) {
		jobId = std::stoi(row[0]);
		resourceId = std::stoi(row[1]);
		rRequired = std::stoi(row[2]);

		this->resourceRequired[jobId - 1][resourceId] = rRequired;
	}
}


DataContainer::DataContainer(ifstream& input)
{
	// Start parsing.
	this->parseJobsNumber(input);
	this->parseMachinesNumber(input);
	this->parseResourcesNumber(input);
	removeLine(input); // White line.

	removeLine(input); // Table header.
	this->parseProcessingTime(input);

	removeLine(input); // Table header.
	this->parseTimeTable(input);

	removeLine(input); // Table header.
	this->parseSetupTimeTable(input);

	removeLine(input); // White line.
	removeLine(input); // Table header.
	this->parseResourceQuantity(input);

	removeLine(input); // Table header.
	this->parseResourceRequired(input);

	input.close();
}

DataContainer::~DataContainer()
{
}

int DataContainer::getJobNumber()
{
	return this->jobs;
}

int DataContainer::getMachineNumber()
{
	return this->machines;
}

int DataContainer::getResourceNumber()
{
	return this->resources;
}

int DataContainer::getProcessingTime(int jobId, int machineId)
{
	return this->processingTime[jobId - 1][machineId];
}

int DataContainer::getDueDate(int jobId)
{
	return this->timeTable[jobId - 1][0];
}

int DataContainer::getReleaseDate(int jobId)
{
	return this->timeTable[jobId - 1][1];
}

int DataContainer::getTardinessPenalty(int jobId)
{
	return this->timeTable[jobId - 1][2];
}

int DataContainer::getSetupTime(int machineId, int beforeJobId, int afterJobId)
{
	return this->setupTimeTable[machineId][beforeJobId - 1][afterJobId - 1];
}

int DataContainer::getResourceQuantity(int resourceType)
{
	return this->resourceQuantity[resourceType];
}

int DataContainer::getResourceRequired(int jobId, int resourceType)
{
	return this->resourceRequired[jobId - 1][resourceType];
}

string DataContainer::getFile()
{
	string file;
	const size_t last_slash_idx = this->file.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
		file = this->file.substr(last_slash_idx + 1);
	return file;
}

string DataContainer::getSolutionFile()
{
	string file = this->file;
	const size_t last_slash_idx = file.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
		file = file.substr(last_slash_idx + 1);
	return file.substr(0, file.size() - 4) + "_sol.csv";
}

void DataContainer::setFile(string file)
{
	this->file = file;
}

DataContainer* DataContainer::fromFile(string filename)
{
	ifstream input(filename);
	DataContainer* dc = new DataContainer(input);
	dc->setFile(filename);
	return dc;
}