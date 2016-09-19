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

multiset<Instant*, Instant::InstantComparator> Resource::getUsage()
{
	return this->used;
}

void Resource::setUsage(multiset<Instant*, Instant::InstantComparator> usage)
{
	this->used = usage;
}

pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> Resource::use(int start, int time, int quantity)
{
	Instant* begin = new Instant(start, quantity);
	Instant* end = new Instant(start + time, -quantity);
	begin->next(end);
	end->next(begin);

	int before = this->used.size();
	multiset<Instant*>::iterator beginIterator = this->used.insert(begin);
	multiset<Instant*>::iterator endIterator = this->used.insert(end);
	int after = this->used.size();

	return pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator>(beginIterator, endIterator);
}

void Resource::free(multiset<Instant*>::iterator i)
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

	multiset<Instant*, Instant::InstantComparator> focusedInstants;

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

pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> Resource::useFirstFreeSlot(int start, int time, int quantity)
{
	start = this->getFirstFreeInstant(start, time, quantity);
	Instant* begin = new Instant(start, quantity);
	Instant* end = new Instant(start + time, -quantity);
	begin->next(end);
	end->next(begin);
	int before = this->used.size();
	multiset<Instant*>::iterator beginIterator = this->used.insert(begin);
	multiset<Instant*>::iterator endIterator = this->used.insert(end);

	return pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator>(beginIterator, endIterator);
}

void Resource::release()
{
	for (Instant* r : this->used)
	{
		delete r;
	}
	this->used.clear();
}
