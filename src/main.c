#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
* @def USER_NUMBER
* The number of users is from 3 to 10, so maximum is 10.
*/
#define USER_NUMBER 10

/**
* @def MAX_USERNAME
* Maximum length of the username, 30 for the name and 1 for the null.
*/
#define MAX_USERNAME 31

/** 
 * @struct User
 * @brief Store the basic information of the user and the appointments.
 */
struct User
{
	char username[MAX_USERNAME];
	struct AppointmentList *accepted;
	struct AppointmentList *rejected;
} user[USER_NUMBER];

/** 
 * @struct AppointmentType
 * @brief Store all the appointment type.
 */
enum AppointmentType
{
	STUDY = 0, ASSIGNMENT, PROJECT, GATHERING
};
const char *AppointmentTypeStr[] = {[STUDY] = "Study", [ASSIGNMENT] = "Assignment", [PROJECT] = "Project",
	[GATHERING] = "Gathering"};

struct Appointment
{
	enum AppointmentType type;
	int caller_id;
	int callee_id[10];
	time_t start;
	time_t end;
	int rescheduled;
	struct Appointment *next;
};	

struct AppointmentList
{
	int count;
	struct Appointment *head;
	struct Appointment *tail;
};

struct AppointmentList* CreateAppointmentList();
struct Appointment* CreateAppointment();

/**
 * @brief Check whether the the new appointment is conflict with the existing appointments that are already in the list.
 * @param[in] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
int CheckConflict(const struct AppointmentList *list, const struct Appointment newItem);

/**
 * @brief Insert a copy of the appointment into the end of the appointment list.
 * @param[out] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
void AddAppointment(struct AppointmentList *list, const struct Appointment *newItem);

/**
 * @brief Insert a copy of the appointment into the sorted appointment list.
 *        Using the start time then end time as the sorting condition.
 * @param[out] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
void AddAppointmentOrdered(struct AppointmentList *list, const struct Appointment *newItem);

/**
 * @brief Compare the start time and then end time of the appointment.
 *        Used to keep the ordered appointment list.
 * @param[in] a Appointment to be compared.
 * @param[in] b Appointment to be compared.
 * @retval <0 a is before b
 * @retval 0 a is equal to b
 * @retval >0 a is after b
 */
int CompareAppointment(const struct Appointment *a, const struct Appointment *b);

/**
 * @brief Return the user id that have the same username.
 * @param username The username that are used to compare.
 * @return The user id that have the same username. Return -1 if user is not found.
 */
int GetUserID(const char *username);

/**
 * @brief Print out the appointment.
 * @param item Appointment to be printed.
 */
void PrintAppointment(const struct Appointment *item);

/**
 * @brief Print out the appointment list.
 * @param list Appointment list to be printed.
 */
void PrintAppointmentList(const struct AppointmentList *list);

void HandleInput(const char *line)
{
	char command[25];
	char caller[MAX_USERNAME];
	int year, month, day;
	int hour, minutes;
	float duration;
	char callee[USER_NUMBER][MAX_USERNAME];
	int callee_count;
	
	struct Appointment *item = CreateAppointment();

	//parse the command
	sscanf(line, "%s", command);
	if(strcmp(command, "addStudy"))
		item->type = STUDY;
	else if(strcmp(command, "addAssignment"))
		item->type = ASSIGNMENT;
	else if(strcmp(command, "addProject"))
		item->type = PROJECT;
	else if(strcmp(command, "addGathering"))
		item->type = GATHERING;
	else if(strcmp(command, "addBatch"))
		return;	//TODO: call the batch handling function
	else if(strcmp(command, "printSchd"))
		return;	//TODO: call the scheduling function
	else if(strcmp(command, "endProgram"))
	{
		printf("Received end program command.\n");
		exit(EXIT_SUCCESS);
	}

	sscanf(line, "%s", caller);
	sscanf(line, "%d-%d-%d", &year, &month, &day);
	sscanf(line, "%d:%d", &hour, &minutes);
	sscanf(line, "%f", &duration);
	for(callee_count=0 ; sscanf(line, "%s", callee[callee_count]) > 0; callee_count++)
		/*empty statement*/;

	item->caller_id = GetUserID(caller);
	struct tm timeinfo;
	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;
	//start time
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minutes;
	//TODO: move to half hour base
	item->start = mktime(&timeinfo);
	//end time
	/*
	int _;
	timeinfo.tm_hour = hour + (int)duration;
	timeinfo.tm_min = minutes + modf(duration, &_)*60;
	item->end = mktime(&timeinfo);
	*/
}

static int NumOfUser;


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
	item->next = NULL;
	return item;
}

void AddAppointment(struct AppointmentList *list, const struct Appointment *newItem)
{
	struct Appointment *item = CreateAppointment();
	*item = *newItem;
	if(!list->head)	//if the list is empty
		list->head = item;
	else
		list->tail->next = item;
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
		if(ptr == NULL)	//insert at the tail
		{
			list->tail->next = item;
			list->tail = item;
		}
		else	//insert after ptr
		{
			item->next = ptr->next;
			ptr->next = item;
		}
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


int GetUserID(const char *username)
{
	for(int i=0; i<NumOfUser; i++)
		if (!strcmp(user[i].username, username))
			return i;
	return -1;
}

void PrintAppointment(const struct Appointment *item)
{
	struct tm *tm_start, *tm_end;
	tm_start = localtime (&item->start);
	tm_end = localtime (&item->end);
	printf("%4d   %02d:%02d   %02d:%02d   %-12s ", tm_start->tm_year+1900, tm_start->tm_hour, tm_start->tm_min,
		tm_end->tm_hour, tm_end->tm_min, AppointmentTypeStr[item->type]);
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

/***************************************************
 * Unit test
 ***************************************************/
void test1()
{
	printf("======================\n");
	printf("TEST 1:\n");
	struct Appointment *a = CreateAppointment();
	a->type = ASSIGNMENT;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	time_t timer;
	time(&timer);
	a->start = timer;
	a->end = timer;
	PrintAppointment(a);	//<<<<<<Testing
}

void test2()
{
	printf("======================\n");
	printf("TEST 2:\n");
	struct Appointment *a = CreateAppointment();
	a->type = PROJECT;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	time_t timer;
	time(&timer);
	a->start = timer;
	a->end = timer;
	struct AppointmentList *list = CreateAppointmentList();
	AddAppointment(list, a);	//<<<<<<Testing
	a->type = STUDY;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	a->start = timer;
	a->end = timer;
	AddAppointment(list, a);	//<<<<<<Testing
	PrintAppointmentList(list);	//<<<<<<Testing
}
void test3()
{
	printf("======================\n");
	printf("TEST 3:\n");
	struct Appointment *a = CreateAppointment();
	a->type = PROJECT;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	time_t timer;
	time(&timer);
	a->start = timer;
	a->end = timer;
	struct AppointmentList *list = CreateAppointmentList();
	AddAppointmentOrdered(list, a);

	struct tm timer2;
	timer2 = *localtime(&timer);
	timer2.tm_hour += 1;
	a->type = STUDY;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	a->start = mktime(&timer2);
	a->end = mktime(&timer2);
	AddAppointmentOrdered(list, a);	//<<<<<<Testing

	timer2 = *localtime(&timer);
	timer2.tm_hour -= 3;
	a->type = STUDY;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	a->start = mktime(&timer2);
	a->end = mktime(&timer2);
	AddAppointmentOrdered(list, a);	//<<<<<<Testing

	timer2 = *localtime(&timer);
	timer2.tm_hour -= 2;
	a->type = STUDY;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	a->start = mktime(&timer2);
	a->end = mktime(&timer2);
	AddAppointmentOrdered(list, a);	//<<<<<<Testing

	timer2 = *localtime(&timer);
	a->type = STUDY;
	a->caller_id = 0;
	a->callee_id[0] = 1;
	a->callee_id[1] = 2;
	a->start = mktime(&timer2);
	a->end = mktime(&timer2);
	AddAppointmentOrdered(list, a);	//<<<<<<Testing
	PrintAppointmentList(list);	//<<<<<<Testing
}

int main(int argc, char* argv[])
{
	if (argc < 4 || argc > 11)
	{
		fprintf(stderr, "Error: The number of users should between 3 and 10.\n");
		return EXIT_FAILURE;
	}
	NumOfUser = argc - 1;
	for (int i=0; i<NumOfUser; i++)
	{
		strcpy(user[i].username, argv[i]);
		user[i].username[0] = toupper(user[i].username[0]);
	}

	test1();
	test2();
	test3();
	return EXIT_SUCCESS;
}