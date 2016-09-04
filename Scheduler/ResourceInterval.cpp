#include "ResourceInterval.h"
#include <stdexcept>

ResourceInterval::ResourceInterval(int start, int duration, int quantity)
{
	/*if (start < 0 || duration < 0 || quantity <= 0) {
		return;
	}*/
	this->start = start;
	this->duration = duration;
	this->end = start + duration;
	this->quantity = quantity;
}

ResourceInterval::~ResourceInterval()
{
}

int ResourceInterval::getStart()
{
	return this->start;
}

int ResourceInterval::getEnd()
{
	return this->end;
}

int ResourceInterval::getQuantity()
{
	return this->quantity;
}

bool ResourceInterval::intersect(ResourceInterval * interval)
{
	if (interval == nullptr) return false;

	return (this->end > interval->start) && (this->start < interval->end);
}

bool ResourceInterval::operator<(const ResourceInterval & other) const
{
	return this->end < other.end;
}
