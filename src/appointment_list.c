#include "appointment_list.h"
#include "user.h"

/***************************************************
 * Implementation
 ***************************************************/
struct AppointmentList* CreateAppointmentList()
{
	struct AppointmentList *list = (struct AppointmentList*)malloc(sizeof(struct AppointmentList));
	if(!list)
	{
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
	return list;
}

struct Appointment* CreateAppointment()
{
	struct Appointment *item = (struct Appointment*)malloc(sizeof(struct Appointment));
	if(!item)
	{
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	for(int i=0; i<USER_NUMBER; i++)
		item->callee_id[i] = -1;
	item->rescheduled = 0;
	item->prev = NULL;
	item->next = NULL;
	return item;
}

void AddAppointment(struct AppointmentList *list, const struct Appointment *newItem)
{
	struct Appointment *item = CreateAppointment();
	*item = *newItem;
	if(!list->head)	//if the list is empty
	{
		list->head = item;
		list->tail = item;
	}
	else
	{
		list->tail->next = item;
	}
	item->prev = list->tail;
	list->tail = item;
	list->count++;
}

void AddAppointmentOrdered(struct AppointmentList *list, const struct Appointment *newItem)
{
	struct Appointment *item = CreateAppointment();
	*item = *newItem;
	struct Appointment *ptr = list->head;
	//if the list is empty
	if(!ptr)
	{
		list->head = item;
		list->tail = item;
	}
	else if(CompareAppointment(item, ptr)<0)	//if insert at the head
	{
		if(!list->head)
			list->head->prev = item;
		item->next = list->head;
		list->head = item;
	}
	else	//insert at middle or at the tail
	{
		while(ptr->next)	//find the insertion position
		{
			if(difftime(item->start, ptr->next->start)<0)
				break;
			ptr = ptr->next;
		}
		if(!ptr)	//insert at the tail
		{
			ptr = list->tail;
			list->tail = item;
		}
		item->prev = ptr;	//insert after ptr
		item->next = ptr->next;
		if(item->next)
			item->next->prev = item;
		ptr->next = item;
	}
	list->count++;
}

int CompareAppointment(const struct Appointment *a, const struct Appointment *b)
{
	if(difftime(a->start, b->start)<0)
		return -1;	//a before b
	else if(difftime(a->start, b->start)==0)
		return difftime(a->end, b->end);
	else
		return 1;
}

void RemoveItemFromList(const struct AppointmentList *list, const struct Appointment *item)
{
	struct Appointment *delItem = list->head;
	while(delItem)
	{
		if(CompareAppointment(delItem, item))
		{
			item->prev->next = item->next;
			item->next->prev = item->prev;
			return;
		}
		delItem = delItem->next;
	}
}

void PrintAppointment(const struct Appointment *item)
{
	struct tm tm_start, tm_end;
	memcpy(&tm_start, localtime (&item->start), sizeof(struct tm));
	memcpy(&tm_end, localtime (&item->end), sizeof(struct tm));
	printf("%4d   %02d:%02d   %02d:%02d   %-12s ", tm_start.tm_year+1900, tm_start.tm_hour,
	 tm_start.tm_min, tm_end.tm_hour, tm_end.tm_min, AppointmentTypeStr[item->type]);
	if(item->rescheduled)
		printf("%-9c ",  'Y');
	else
		printf("%-9c ", 'N');
	if(item->callee_id[0] == -1)
		printf("-");
	for(int i=0; i<USER_NUMBER; i++)
	{
		if(item->callee_id[i]==-1)
			break;
		printf("%s ", user[item->callee_id[i]].username);
	}
	printf("\n");
}

void PrintAppointmentList(const struct AppointmentList *list)
{
	struct Appointment *ptr = list->head;
	while(ptr!=NULL)
	{
		PrintAppointment(ptr);
		ptr = ptr->next;
	}
}

int isConflict(const struct Appointment *a, const struct Appointment *b)
{
	return !(difftime(a->end, b->start)<0 || 	//a before b
		difftime(a->start, b->end)>0); 	//a after b
}