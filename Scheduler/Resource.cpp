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

	int end = start + time;
	int used = 0;
	int usedInitial = 0;
	int totalUsed = 0;

	InstantSet focusedInstants;

	for (Instant* usedInstant : this->used)
	{
		if (usedInstant->getTime() <= start) {
			// Get initial count. Can contribute only positive instant with time greater than start.
			// It means there was reasoures used before "start" instant.
			if (usedInstant->getQuantity() > 0 && usedInstant->next()->getTime() > start) {
				// Here use only start instant.
				focusedInstants.insert(usedInstant->next());

				used += usedInstant->getQuantity();
				usedInitial += used;
				totalUsed = used;

				if (this->quantity - totalUsed < quantity) {
					break;
				}
			}
		}
		else if (usedInstant->getTime() < end) {
			if (usedInstant->getQuantity() > 0) {
				focusedInstants.insert(usedInstant->next());
			}

			used += usedInstant->getQuantity();
			totalUsed = max(totalUsed, used);

			if (this->quantity - totalUsed < quantity) {
				break;
			}
		}
		else {
			break;
		}
	}

	if (this->quantity - totalUsed >= quantity) {
		return start;
	}

	int freedResources = 0;
	for (Instant* candidate : focusedInstants) {
		// All end instant, so use - to increment.
		freedResources -= candidate->getQuantity();
		if (this->quantity - usedInitial + freedResources >= quantity) {
			return this->getFirstFreeInstant(end, time, quantity);
		}
	}

	return this->getFirstFreeInstant((*focusedInstants.rbegin())->getTime(), time, quantity);
}

InstantSetIteratorPair Resource::useFirstFreeSlot(int start, int time, int quantity)
{
	start = this->getFirstFreeInstant(start, time, quantity);
	Instant* begin = new Instant(start, quantity);
	Instant* end = new Instant(start + time, -quantity);
	begin->next(end);
	end->next(begin);
	int before = this->used.size();
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
