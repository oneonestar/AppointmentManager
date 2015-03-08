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

void Schedual_FCFS(struct AppointmentList *inputList)
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