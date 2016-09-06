#pragma once
#include "Job.h"

using namespace std;

class Instant
{
private:
	int time;
	int quantity;
	Job* job;
	Instant* sibling;
public:
	Instant(int time, int quantity);
	Instant(int time, int quantity, Job* job);
	~Instant();

	int getTime() const;
	int getQuantity() const;
	Job* getJob() const;
	void setJob(Job* next);
	void setSibling(Instant* sibling);

	Instant* next();
	bool operator<(const Instant& other) const;
};

