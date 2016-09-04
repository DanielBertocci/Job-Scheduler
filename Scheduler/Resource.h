#pragma once
#include <set>
#include <algorithm>
#include <string>
#include "ResourceInterval.h"

using namespace std;

class Resource
{
private:
	int id;
	int quantity;

	// [time][used]
	set<ResourceInterval*> used;

public:
	Resource(int id, int quantity);
	~Resource();

	int getId() const;
	set<ResourceInterval*> getUsage();
	ResourceInterval* use(int start, int time, int quantity);
	void free(ResourceInterval* i);
	ResourceInterval* getFirstFreeSlot(int start, int time, int quantity);
	ResourceInterval* useFirstFreeSlot(int start, int time, int quantity);
	void release();
};

