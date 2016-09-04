#include "Resource.h"



Resource::Resource(int id, int quantity)
{
	this->id = id;
	this->quantity = quantity;
}


Resource::~Resource()
{
}

int Resource::getId() const
{
	return this->id;
}

set<ResourceInterval*> Resource::getUsage()
{
	return this->used;
}

ResourceInterval* Resource::use(int start, int time, int quantity)
{
	ResourceInterval* interval = new ResourceInterval(start, time, quantity);
	if (quantity > this->quantity) {
		throw runtime_error("Something was wrong: something require more Resource#" + to_string(this->id) + " than available.");
	}
	this->used.insert(interval);
	return interval;
}

void Resource::free(ResourceInterval* i)
{
	this->used.erase(i);
}

ResourceInterval * Resource::getFirstFreeSlot(int start, int time, int quantity)
{
	ResourceInterval* interval = new ResourceInterval(start, time, quantity);
	if (quantity > this->quantity) {
		throw runtime_error("Something was wrong: something require more Resource#" + to_string(this->id) + " than available.");
	}

	int used = 0;
	ResourceInterval* prev = nullptr;

	for (ResourceInterval* usedInterval : this->used)
	{
		if (interval->intersect(usedInterval)) {
			if (usedInterval->intersect(prev)) {
				used += usedInterval->getQuantity();
			}
			else {
				used = usedInterval->getQuantity();
			}

			if (used + quantity > this->quantity) {
				return this->getFirstFreeSlot(usedInterval->getEnd(), time, quantity);
			}

			prev = usedInterval;
		}
	}
	return interval;
}

ResourceInterval* Resource::useFirstFreeSlot(int start, int time, int quantity)
{
	ResourceInterval* interval = new ResourceInterval(start, time, quantity);
	if (quantity > this->quantity) {
		throw runtime_error("Something was wrong: something require more Resource#" + to_string(this->id) + " than available.");
	}

	int used = 0;
	ResourceInterval* prev = nullptr;

	for (ResourceInterval* usedInterval : this->used)
	{
		if (interval->intersect(usedInterval)) {
			if (usedInterval->intersect(prev)) {
				used += usedInterval->getQuantity();
			}
			else {
				used = usedInterval->getQuantity();
			}

			if (used + quantity > this->quantity) {
				return this->useFirstFreeSlot(usedInterval->getEnd(), time, quantity);
			}

			prev = usedInterval;
		}
	}

	this->used.insert(interval);
	return interval;
}

void Resource::release()
{
	for (ResourceInterval* r : this->used)
	{
		delete r;
	}
	this->used.clear();
}
