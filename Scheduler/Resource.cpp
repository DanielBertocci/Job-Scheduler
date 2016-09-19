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
	
	return pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator>(beginIterator,endIterator);
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
	int totalUsed = 0;
	Instant* candidate = nullptr;

	multiset<Instant*, Instant::InstantComparator> focusedInstants;

	for (Instant* usedInstant : this->used)
	{
		if (usedInstant->getTime() <= start) {
			// Get initial count. Can contribute only positive instant with time greater than start.
			// It means there was reasoures used before "start" instant.
			if (usedInstant->getQuantity() > 0 && usedInstant->next()->getTime() > start) {
				// Here use only start instant.
				used += usedInstant->getQuantity();
				totalUsed = used;

				if (this->quantity - used >= quantity) {
					if(candidate == nullptr || usedInstant->next()->getTime() < candidate->getTime()) {
						candidate = usedInstant->next();
					}
				}
			}
		}
		else if(usedInstant->getTime() < end){
			used += usedInstant->getQuantity();
			totalUsed = max(totalUsed, used);

			// Search smart begin for new search.
			if (this->quantity - used >= quantity) {
				candidate = usedInstant;
			}
			else {
				candidate = usedInstant->next();
			}
		}
		else {
			break;
		}
	}

	if (this->quantity - totalUsed >= quantity) {
		return start;
	}

	if (candidate != nullptr && candidate->getTime() > start) {
		end = min(end, candidate->getTime());
	}

	return this->getFirstFreeInstant(end, time, quantity);
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
