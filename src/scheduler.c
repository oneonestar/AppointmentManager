#include "appointment_list.h"
#include "user.h"

#include <unistd.h>

#include "scheduler.h"

int IsAllAvailable(const struct Appointment *item)
{
	if(!item)
		return 0;
	struct AppointmentList *temp_list;
	//check caller timetable
	temp_list = ConflictInList(user[item->caller_id].accepted, item);
	if(temp_list->count)
		return 0;
	//check callees timetable
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		temp_list = ConflictInList(user[item->callee_id[i]].accepted, item);
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
	temp_list = ConflictInList(user[item->caller_id].accepted, item);
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
		temp_list = ConflictInList(user[item->callee_id[i]].accepted, item);
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
	temp_list = ConflictInList(user[item->caller_id].accepted, item);
	RemoveListFromList(user[item->caller_id].accepted, temp_list);
	//add to accept and reject lsit
	AddAppointmentOrdered(user[item->caller_id].accepted, item);
	AddAppointmentOrderedFromList(user[item->caller_id].rejected, temp_list);
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		//delete old appointments from accepted list
		temp_list = ConflictInList(user[item->callee_id[i]].accepted, item);
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

/** For summary, set the appointment->is_accepted in input list according to the user's accept list. */
static void SetAppointmentAccepted(struct AppointmentList *inputList)
{
	for(int i=0; i<NumOfUser; i++)
	{
		struct Appointment *ptr = user[i].accepted->head;
		while(ptr)
		{
			GetAppointmentById(inputList, ptr->id)->is_accepted = 1;
			ptr = ptr->next;
		}
	}
}

static struct AppointmentList* GetEmptyTimeSlotInDay(struct AppointmentList *userList, time_t date)
{
	struct AppointmentList *empty_list = CreateAppointmentList();
	//18:00-22:00
	struct tm timeinfo, timeinfo_tmp;
	timeinfo = *localtime (&date);

	//foreach 18:00-22:00 half hour timeslot
	timeinfo.tm_hour = 18;
	while(timeinfo.tm_hour<22)
	{
		struct Appointment *item = CreateAppointment();
		//first half hour
		timeinfo_tmp = timeinfo;	//because mktime could modify the value
		item->start = mktime(&timeinfo_tmp);
		timeinfo.tm_min = 30;
		timeinfo_tmp = timeinfo;	//because mktime could modify the value
		item->end = mktime(&timeinfo_tmp);
		if(!IsConflictInList(userList, item))
			AddAppointmentOrdered(empty_list, item);


		//second half hour
		timeinfo_tmp = timeinfo;	//because mktime could modify the value
		item->start = mktime(&timeinfo_tmp);
		timeinfo.tm_hour++;
		timeinfo.tm_min = 0;
		timeinfo_tmp = timeinfo;	//because mktime could modify the value
		item->end = mktime(&timeinfo_tmp);
		if(!IsConflictInList(userList, item))
			AddAppointmentOrdered(empty_list, item);
	}
	return empty_list;
}

static struct AppointmentList* GetEmptyTimeSlotInRange(struct AppointmentList *userList, time_t start_date, time_t end_date)
{
	struct AppointmentList *empty_list = CreateAppointmentList();
	struct tm timeinfo, timeinfo_tmp;

	//set the time start<end so that we can use difftime() to compare the date.
	timeinfo = *localtime (&start_date);
	timeinfo.tm_hour = 1;
	timeinfo_tmp = timeinfo;
	start_date = mktime(&timeinfo_tmp);

	timeinfo = *localtime (&end_date);
	timeinfo.tm_hour = 2;
	timeinfo_tmp = timeinfo;
	end_date = mktime(&timeinfo_tmp);

	timeinfo = *localtime (&start_date);
	while(difftime(start_date, end_date)<0)
	{
		AddAppointmentFromList(empty_list, GetEmptyTimeSlotInDay(userList, start_date));

		timeinfo.tm_mday++;
		timeinfo_tmp = timeinfo;
		start_date = mktime(&timeinfo_tmp);
	}
	return empty_list;
}

static time_t GetEarliestStartTime(struct AppointmentList *list)
{
	struct Appointment *item = list->head;
	time_t earliest = item->start;
	while(item)
	{
		if(difftime(item->start, earliest)<0)
			earliest = item->start;
		item = item->next;
	}
	return earliest;
}

static time_t GetLatestEndTime(struct AppointmentList *list)
{
	struct Appointment *item = list->head;
	time_t latest = item->end;
	while(item)
	{
		if(difftime(latest, item->end)<0)
			latest = item->end;
		item = item->next;
	}
	return latest;
}

static struct AppointmentList* GetContinueTimeslotFromList(const struct AppointmentList *list, time_t duration)
{
	struct Appointment *ptr;
	struct Appointment *item = list->head;
	struct AppointmentList *ret_list = CreateAppointmentList();
	int timeslot = duration / 60 / 30 - 1;	//how many half hour
	while(item)
	{
		ptr = item;
		for(int i=0; i<timeslot; i++)
			ptr = ptr->next;
		if(!ptr)
			return ret_list;
		if(difftime(ptr->end, item->start)==duration)
			AddAppointmentOrdered(ret_list, item);
		item = item->next;
	}
	return ret_list;
}

struct Summary* Schedual_FCFS(struct AppointmentList *inputList)
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
		ptr = ptr->next;
	}

	//Summary
	struct Summary *summary = (struct Summary *)malloc(sizeof(struct Summary));
	summary->start = GetEarliestStartTime(inputList);
	summary->end = GetLatestEndTime(inputList);
	
	SetAppointmentAccepted(inputList);
	ptr = inputList->head;
	while(ptr)
	{
		if(ptr->is_accepted)
			summary->total_accepted++;
		else
			summary->total_rejected++;
		ptr = ptr->next;
	}
	for(int i=0; i<NumOfUser; i++)
	{
		summary->accepted[i] = user[i].accepted->count;
		summary->rejected[i] = user[i].rejected->count;
		summary->empty_timeslot[i] = GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end)->count;

		// PrintAppointmentList(GetContinueTimeslotFromList(GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end), 2*60*30));
		// PrintAppointmentList(GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end));
	}
	return summary;
}

struct Summary* Schedual_PRIO(struct AppointmentList *inputList)
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

	//Summary
	struct Summary *summary = (struct Summary *)malloc(sizeof(struct Summary));
	summary->start = GetEarliestStartTime(inputList);
	summary->end = GetLatestEndTime(inputList);
	
	SetAppointmentAccepted(inputList);
	ptr = inputList->head;
	while(ptr)
	{
		if(ptr->is_accepted)
			summary->total_accepted++;
		else
			summary->total_rejected++;
		ptr = ptr->next;
	}
	for(int i=0; i<NumOfUser; i++)
	{
		summary->accepted[i] = user[i].accepted->count;
		summary->rejected[i] = user[i].rejected->count;
		summary->empty_timeslot[i] = GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end)->count;
		// PrintAppointmentList(GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end));
	}
	return summary;
}

struct Summary* Schedual_OPTI(struct AppointmentList *inputList)
{
	struct Summary *summary = Schedual_PRIO(inputList);
	time_t day = 60*60*24;

	//For each user, try to reschedule their rejected jobs
	for(int i=0; i<NumOfUser; i++)
	{
		struct Appointment *item = user[i].rejected->head;
		NEXT_ITEM:
		while(item)
		{
			time_t ori_start = item->start;
			time_t ori_end = item->end;
			time_t duration = difftime(item->end, item->start);
			struct AppointmentList *list = GetEmptyTimeSlotInRange(user[i].accepted, item->start, item->start+3*day);
			struct AppointmentList *c_list = GetContinueTimeslotFromList(list, duration);

			struct Appointment *timeslot = c_list->head;
			while(timeslot)
			{
				item->start = timeslot->start;
				item->end = item->start + duration;

				if(IsAllAvailable(item))
				{
					item->rescheduled = 1;
					AddToAllAccept(item);
					struct Appointment *temp = item;
					item = item->next;
					for(int j=0; j<NumOfUser; j++)
						RemoveItemFromList(user[j].rejected, temp);
					goto NEXT_ITEM;
				}
				timeslot = timeslot->next;
			}
			item = item->next;
		}
	}

	//Re-calculate the summary
	memset(summary, 0, sizeof(struct Summary));
	summary->start = GetEarliestStartTime(inputList);
	summary->end = GetLatestEndTime(inputList);
	SetAppointmentAccepted(inputList);
	struct Appointment *ptr = inputList->head;
	while(ptr)
	{
		if(ptr->is_accepted)
			summary->total_accepted++;
		else
			summary->total_rejected++;
		ptr = ptr->next;
	}
	for(int i=0; i<NumOfUser; i++)
	{
		summary->accepted[i] = user[i].accepted->count;
		summary->rejected[i] = user[i].rejected->count;
		summary->empty_timeslot[i] = GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end)->count;
		// PrintAppointmentList(GetEmptyTimeSlotInRange(user[i].accepted, summary->start, summary->end));
	}
	return summary;
}

static int rdn(int y, int m, int d) {
    if (m < 3)
        y--, m += 12;
    return 365*y + y/4 - y/100 + y/400 + (153*m - 457)/5 + d - 306;
}

void PrintSummary(struct Summary *summary)
{
	float total = summary->total_accepted+summary->total_rejected;
	struct tm timeinfo, timeinfo2;
	int days;
	
	printf("Performance:\n");
	timeinfo = *localtime(&summary->start);
	printf("Date start: %4d-%02d-%02d\n", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday);
	timeinfo2 = *localtime(&summary->end);
	printf("Date end: %4d-%02d-%02d\n\n", timeinfo2.tm_year+1900, timeinfo2.tm_mon+1, timeinfo2.tm_mday);

	printf("Total Number of Appointment Assigned: %d (%.1f%%)\n", summary->total_accepted, summary->total_accepted/total*100);
	printf("Total Number of Appointment Rejected: %d (%.1f%%)\n", summary->total_rejected, summary->total_rejected/total*100);

	days = rdn(timeinfo2.tm_year, timeinfo2.tm_mon+1, timeinfo2.tm_mday) - rdn(timeinfo.tm_year, timeinfo.tm_mon+1, timeinfo.tm_mday) + 1;
	printf("Utilization of Time Slot: (%d days)\n", days+1);
	for(int i=0; i<NumOfUser; i++)
	{
		printf("    %-10s - %.1f%%\n", user[i].username, (days*2*4-(float)summary->empty_timeslot[i])/(days*2*4)*100);	//each day have 2*8 timeslots
	}
	
}