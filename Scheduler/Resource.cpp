#include "Resource.h"
#include <Windows.h>
#include "TemplateResource.h"

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

int Resource::getQuantity() const
{
	return this->quantity;
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

		if (used <= this->quantity - quantity) {
			if (beginInstant == nullptr) {
				beginInstant = usedInstant;
			}
		}
		else {
			beginInstant = nullptr;
		}

		if (beginInstant != nullptr && usedInstant->getTime() - beginInstant->getTime() >= time) {
			return beginInstant->getTime();
		}

		used += usedInstant->getQuantity();

		// Next iterator.
		++instantIterator;
	}
	
	if (beginInstant != nullptr) {
		return beginInstant->getTime();
	}
	else {
		return usedInstant->getTime();
	}
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

void Resource::printUsage()
{
	stringstream ss;
	map<int, int> table;
	int prev = 0;
	int counter = 0;
	ss << "['Instant','Usage','Max']," << endl;
	for (Instant* i : this->used)
	{
		counter += i->getQuantity();
		table[i->getTime()] = counter;
	}
	for (auto p : table) {
		if (table[prev] != p.second) {
			ss << "[" << p.first << "," << table[prev] << "," << this->getQuantity() << "]," << endl;
		}
		ss << "[" << p.first << "," << p.second << "," << this->getQuantity() << "]," << endl;
		prev = p.first;
	}
	this->graphArea("solution_usage_area_chart" + to_string(this->getId()) + ".html", ss.str());
	table.clear();
	ss.clear();
	counter = 0;
	prev = 0;
}

void Resource::graphArea(string filePath, string jsData)
{
	HRSRC graphTemplateResource = FindResource(NULL, MAKEINTRESOURCE(IDR_HTML_AREA_CHART), RT_HTML);
	HGLOBAL graphTemplateResourceData = LoadResource(NULL, graphTemplateResource);
	string graphTemplate((char*)LockResource(graphTemplateResourceData));


#include <sstream>
	auto pos = graphTemplate.find("$DATA");
	if (pos != string::npos) {
		graphTemplate.replace(pos, 5, jsData);
	}


	ofstream out(filePath, ios::out | ios::binary);
	out << graphTemplate;
	out.close();
}
