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

InstantSet Resource::getUsage()
{
	return this->used;
}

void Resource::setUsage(InstantSet usage)
{
	this->used = usage;
}

InstantSetIteratorPair Resource::use(int start, int time, int quantity)
{
	Instant* begin = new Instant(start, quantity);
	Instant* end = new Instant(start + time, -quantity);
	begin->next(end);
	end->next(begin);

	int before = this->used.size();
	InstantSetIterator beginIterator = this->used.insert(begin);
	InstantSetIterator endIterator = this->used.insert(end);
	int after = this->used.size();

	return InstantSetIteratorPair(beginIterator, endIterator);
}

void Resource::free(InstantSetIterator i)
{
	this->used.erase(i);
}

int Resource::getFirstFreeInstant(int start, int time, int quantity)
{
	if (quantity > this->quantity) {
		throw runtime_error("Something was wrong: something require more Resource#" + to_string(this->id) + " than available.");
	}

	int used = 0;

	InstantSetIterator instantIterator = this->used.begin();
	Instant* usedInstant = nullptr;
	Instant* beginInstant = new Instant(start, quantity);

	while (instantIterator != this->used.end() && (*instantIterator)->getTime() <= start) {
		used += (*instantIterator)->getQuantity();
		++instantIterator;
	}

	while (instantIterator != this->used.end()) {
		usedInstant = *instantIterator;
		if (used <= this->quantity - quantity && beginInstant != nullptr && usedInstant->getTime() - beginInstant->getTime() >= time) {
			return beginInstant->getTime();
		}

		used += usedInstant->getQuantity();

		if (used <= this->quantity - quantity) {
			if (beginInstant == nullptr) {
				beginInstant = usedInstant;
			}
		}
		else {
			beginInstant = nullptr;
		}

		// Next iterator.
		++instantIterator;
	}
	return beginInstant->getTime();
}

InstantSetIteratorPair Resource::useFirstFreeSlot(int start, int time, int quantity)
{
	start = this->getFirstFreeInstant(start, time, quantity);
	Instant* begin = new Instant(start, quantity);
	Instant* end = new Instant(start + time, -quantity);
	begin->next(end);
	end->next(begin);
	InstantSetIterator beginIterator = this->used.insert(begin);
	InstantSetIterator endIterator = this->used.insert(end);

	return InstantSetIteratorPair(beginIterator, endIterator);
}

void Resource::release()
{
	for (Instant* r : this->used)
	{
		delete r;
	}
	this->used.clear();
}
