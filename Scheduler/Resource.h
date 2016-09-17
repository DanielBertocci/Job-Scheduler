#pragma once
#include <set>
#include <algorithm>
#include <string>
#include "Instant.h"

using namespace std;

class Resource
{
private:
	int id;
	int quantity;
	multiset<Instant*, Instant::InstantComparator> used;

public:
	Resource(int id, int quantity);
	~Resource();

	int getId() const;
	multiset<Instant*, Instant::InstantComparator> getUsage();
	pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> use(int start, int time, int quantity);
	void free(multiset<Instant*>::iterator i);
	int getFirstFreeInstant(int start, int time, int quantity);
	pair<multiset<Instant*>::iterator, multiset<Instant*>::iterator> useFirstFreeSlot(int start, int time, int quantity);
	void release();
};

