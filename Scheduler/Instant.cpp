#include "Instant.h"

Instant::Instant(int time, int quantity)
{
	this->time = time;
	this->quantity = quantity;
	this->jobId = -1;
}

Instant::Instant(int time, int quantity, int job)
{
	this->time = time;
	this->quantity = quantity;
	this->jobId = job;
}

Instant::~Instant()
{
}

int Instant::getTime() const
{
	return this->time;
}

int Instant::getQuantity() const
{
	return this->quantity;
}

void Instant::shiftLeft(int amount)
{
	this->time -= amount;
}

int Instant::getJob() const
{
	return this->jobId;
}

void Instant::setJob(int jobId)
{
	this->jobId = jobId;
}

void Instant::next(Instant * sibling)
{
	this->sibling = sibling;
}

bool Instant::operator<(const Instant & other) const
{
	return this->time < other.time;
}

Instant * Instant::next()
{
	return this->sibling;
}