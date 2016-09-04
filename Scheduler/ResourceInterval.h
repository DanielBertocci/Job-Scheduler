#pragma once
class ResourceInterval
{
private:
	int start;
	int end;
	int duration;
	int quantity;
public:
	ResourceInterval(int start, int duration, int quantity);
	~ResourceInterval();

	int getStart();
	int getEnd();
	int getQuantity();
	bool intersect(ResourceInterval* interval);
	bool operator<(const ResourceInterval& other) const;
};

