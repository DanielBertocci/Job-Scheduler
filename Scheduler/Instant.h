#pragma once
//#include "Job.h"

using namespace std;

class Instant
{
private:
	int time;
	int quantity;
	//Job* job;
	Instant* sibling;
public:
	Instant(int time, int quantity);
	//Instant(int time, int quantity, Job* job);
	~Instant();

	struct InstantComparator
	{
		bool operator()(const Instant* a, const Instant* b) const {
			return a->time < b->time;
		}
	};

	int getTime() const;
	int getQuantity() const;
	/*Job* getJob() const;
	void setJob(Job* next);*/

	Instant* next();
	void next(Instant* sibling);

	bool operator<(const Instant& other) const;
};


