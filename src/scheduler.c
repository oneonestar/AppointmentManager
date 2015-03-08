#include "appointment_list.h"
#include "user.h"

#include <unistd.h>

#include "scheduler.h"

int IsAllAvailable(const struct Appointment *item)
{
	struct AppointmentList *temp_list;
	//check caller timetable
	temp_list = IsConflictInList(user[item->caller_id].accepted, item);
	if(temp_list->count)
		return 0;
	//check callees timetable
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		temp_list = IsConflictInList(user[item->callee_id[i]].accepted, item);
		if(temp_list->count)
			return 0;
	}
	return 1;
}

int IsAllAvailablePriority(const struct Appointment *item)
{
	struct AppointmentList *temp_list;
	struct Appointment *ptr;
	//check caller timetable
	temp_list = IsConflictInList(user[item->caller_id].accepted, item);
	ptr = temp_list->head;
	while(ptr)
	{
		if(CompareAppointmentPriority(item, ptr)>=0)	//if item has equal or lower priority
			return 0;	//not available
		ptr = ptr->next;
	}
	//check callees timetable
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		//TODO: may need refactoring
		temp_list = IsConflictInList(user[item->callee_id[i]].accepted, item);
		ptr = temp_list->head;
		while(ptr)
		{
			if(CompareAppointmentPriority(item, ptr)>=0)	//if item has equal or lower priority
				return 0;	//not available
			ptr = ptr->next;
		}
	}
	return 1;	//available
}

void AddToAllAccept(const struct Appointment *item)
{
	AddAppointmentOrdered(user[item->caller_id].accepted, item);
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		AddAppointmentOrdered(user[item->callee_id[i]].accepted, item);
	}
}

void AddToAllAcceptForced(const struct Appointment *item)
{
	struct AppointmentList *temp_list;
	//caller
	//delete old appointments from accepted list
	temp_list = IsConflictInList(user[item->caller_id].accepted, item);
	RemoveListFromList(user[item->caller_id].accepted, temp_list);
	//add to accept and reject lsit
	AddAppointmentOrdered(user[item->caller_id].accepted, item);
	AddAppointmentOrderedFromList(user[item->caller_id].rejected, temp_list);
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		//delete old appointments from accepted list
		temp_list = IsConflictInList(user[item->callee_id[i]].accepted, item);
		RemoveListFromList(user[item->callee_id[i]].accepted, temp_list);
		//add to accept and reject lsit
		AddAppointmentOrdered(user[item->callee_id[i]].accepted, item);
		AddAppointmentOrderedFromList(user[item->callee_id[i]].rejected, temp_list);
	}
}

void AddToAllReject(const struct Appointment *item)
{
	AddAppointmentOrdered(user[item->caller_id].rejected, item);
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		AddAppointmentOrdered(user[item->callee_id[i]].rejected, item);
	}
}

void Schedual_FCFS(const struct AppointmentList *inputList)
{
	struct Appointment *ptr = inputList->head;
	while(ptr)
	{
		if(IsAllAvailable(ptr))
		{
			AddToAllAccept(ptr);
		}
		else
		{
			AddToAllReject(ptr);
		}
		// PrintAppointmentList(inputList);
		// PrintAllUser();
		// sleep(3);
		ptr = ptr->next;
	}
}

void Schedual_PRIO(const struct AppointmentList *inputList)
{
	struct Appointment *ptr = inputList->head;
	while(ptr)
	{
		if(IsAllAvailablePriority(ptr))
		{
			AddToAllAcceptForced(ptr);
		}
		else
		{
			AddToAllReject(ptr);
		}
		ptr = ptr->next;
	}
}