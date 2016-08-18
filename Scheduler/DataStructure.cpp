#include "DataStructure.h"

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
	int **matrix = new int*[p];
	for (int i = 0; i < p; ++i)
		matrix[i] = new int[q];

	for (int i = 0; i < p; ++i)
		for (int j = 0; j < q; ++j)
			matrix[i][j] = -1;

	return matrix;
}

void DataStructure::parseJobsNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Jobs") {
		throw invalid_argument("File bad format: the first line is required to be the jobs number.");
	}

	this->jobs = stoi(row[1]);
}

void DataStructure::parseMachinesNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Machines") {
		throw invalid_argument("File bad format: the second line is required to be the machines number.");
	}

	this->machines = stoi(row[1]);
}

void DataStructure::parseResourcesNumber(istream & input)
{
	vector<string> row;
	getRow(input, row);

	if (row[0] != "#Resources") {
		throw invalid_argument("File bad format: the third line is required to be the resources number.");
	}

	this->resources = stoi(row[1]);
}

void DataStructure::parseProcessingTime(istream & in)
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

void DataStructure::parseTimeTable(istream & in)
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

		timeTable[jobId - 1][0] = readyTime;
		timeTable[jobId - 1][1] = dueDate;
		timeTable[jobId - 1][2] = tardinessPenalty;
	}
}

void DataStructure::parseSetupTimeTable(istream & in)
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

void DataStructure::parseResourceQuantity(istream & in)
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

void DataStructure::parseResourceRequired(istream & in)
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


DataStructure::DataStructure(ifstream& input)
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

DataStructure::~DataStructure()
{
}

int DataStructure::getProcessingTime(int jobId, int machineId)
{
	return this->processingTime[jobId - 1][machineId];
}

int DataStructure::getDueDate(int jobId)
{
	return this->timeTable[jobId - 1][0];
}

int DataStructure::getReleaseDate(int jobId)
{
	return this->timeTable[jobId - 1][1];
}

int DataStructure::getTardinessPenalty(int jobId)
{
	return this->timeTable[jobId - 1][2];
}

int DataStructure::getSetupTime(int machineId, int beforeJobId, int afterJobId)
{
	return this->setupTimeTable[machineId][beforeJobId][afterJobId];
}

int DataStructure::getResourceQuantity(int resourceType)
{
	return this->resourceQuantity[resourceType];
}

int DataStructure::getResourceRequired(int jobId, int resourceType)
{
	return this->resourceRequired[jobId][resourceType];
}

DataStructure* DataStructure::fromFile(string filename)
{
	ifstream input(filename);
	return new DataStructure(input);
}