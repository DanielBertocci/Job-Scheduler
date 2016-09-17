#include "Instant.h"

Instant::Instant(int time, int quantity)
{
	this->time = time;
	this->quantity = quantity;
	//this->job = nullptr;
}

//Instant::Instant(int time, int quantity, Job * job)
//{
//	this->time = time;
//	this->quantity = quantity;
//	this->job = job;
//}

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

//Job * Instant::getJob() const
//{
//	return this->job;
//}

//void Instant::setJob(Job * next)
//{
//	this->job = job;
//}

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