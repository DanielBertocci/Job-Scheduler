#pragma once
#include <set>
#include <algorithm>
#include <string>
#include "Instant.h"

using namespace std;

// Types definition.
typedef multiset<Instant*, Instant::InstantComparator> InstantSet;
typedef multiset<Instant*, Instant::InstantComparator>::iterator InstantSetIterator;
typedef pair<InstantSetIterator, InstantSetIterator> InstantSetIteratorPair;

class Resource
{
private:
	int id;
	int quantity;
	InstantSet used;

public:
	Resource(int id, int quantity);
	~Resource();

	// Getters.
	int getId() const;
	InstantSet getUsage();

	// Setters.
	void setUsage(InstantSet usage);

	// Actions.
	InstantSetIteratorPair use(int start, int time, int quantity);
	InstantSetIteratorPair useFirstFreeSlot(int start, int time, int quantity);
	int getFirstFreeInstant(int start, int time, int quantity);
	void free(InstantSetIterator i);
	void release();
};

