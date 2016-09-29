#pragma once

#include <set>
#include "Job.h"

using namespace std;

class Job;

class ScheduleList
{
protected:
	ScheduleList();

public:
	~ScheduleList();

	multiset<Job*> schedule;
	static ScheduleList& getInstance();

};

