#pragma once

using namespace std;

class Instant
{

private:
	int time;
	int quantity;
	int jobId;
	Instant* sibling;
public:
	Instant(int time, int quantity);
	Instant(int time, int quantity, int jobId);
	~Instant();

	struct InstantComparator
	{
		bool operator()(const Instant* a, const Instant* b) const {
			return a->time < b->time || (a->time == b->time && a->quantity < b->quantity);
		}
	};

	int getTime() const;
	int getQuantity() const;
	void shiftLeft(int amount);
	int getJob() const;
	void setJob(int jobId);

	Instant* next();
	void next(Instant* sibling);

	bool operator<(const Instant& other) const;
};


