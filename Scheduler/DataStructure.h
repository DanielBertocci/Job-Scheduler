#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

class DataStructure
{
private:
	int jobs;
	int machines;
	int resources;

	// [job][machine]
	int** processingTime;

	// Contains in order Dj, Rj, Wj.
	// [job][time_type]
	int** timeTable;

	// [machine][job_i][job_j]
	int*** setupTimeTable;

	// [resource_type]
	int* resourceQuantity;

	// [job][resource_type]
	int** resourceRequired;

	// Parsing file functions.
	void parseJobsNumber(istream& input);
	void parseMachinesNumber(istream& input);
	void parseResourcesNumber(istream& input);
	void parseProcessingTime(istream& in);
	void parseTimeTable(istream& in);
	void parseSetupTimeTable(istream& in);
	void parseResourceQuantity(istream& in);
	void parseResourceRequired(istream& in);

public:
	DataStructure(ifstream& input);
	~DataStructure();

	// Time getters.
	int getProcessingTime(int jobId, int machineId);
	int getDueDate(int jobId);
	int getReleaseDate(int jobId);
	int getTardinessPenalty(int jobId);
	int getSetupTime(int beforeJobId, int afterJobId, int machineId);

	// Resources.
	int getResourceQuantity(int resourceType);
	int getResourceRequired(int jobId, int resourceType);

	// Parsing file.
	static DataStructure* fromFile(string filename);
};

