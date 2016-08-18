#pragma once
class Job
{
public:
	int id;
	int readyTime;
	int start;
	int end;
	int index;

	Job(int id);
	~Job();
};

