#include "ScheduleList.h"



ScheduleList::ScheduleList()
{
}


ScheduleList::~ScheduleList()
{
}

ScheduleList& ScheduleList::getInstance()
{
	static ScheduleList scheduleList;
	return scheduleList;
}
