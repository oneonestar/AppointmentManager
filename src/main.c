#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "appointment_list.h"
#include "scheduler.h"
#include "user.h"

extern int NumOfUser;
extern struct User user[USER_NUMBER];

struct AppointmentList *inputList;

void HandleInput(const char *line);
void HandleSchedule(const char *algorithm);
void inputLoop(FILE *stream);

void HandleSchedule(const char *algorithm)
{
	struct Summary *summary;
	//TODO: remove items from list
	for (int i=0; i<NumOfUser; i++)
	{
		user[i].accepted = CreateAppointmentList();
		user[i].rejected = CreateAppointmentList();
	}
	if(!strcmp(algorithm, "-fcfs"))
		summary = Schedual_FCFS(inputList);
	else if(!strcmp(algorithm, "-prio"))
		summary = Schedual_PRIO(inputList);
	else if(!strcmp(algorithm, "-opti"))
		return;
	PrintAllUser();
	PrintSummary(summary);
}

void HandleInput(const char *line)
{
	char command[25];
	char caller[MAX_USERNAME];
	int year, month, day;
	int hour, minutes;
	float duration;
	int callee_count = 0;
	char *pch;
	
	struct Appointment *item = CreateAppointment();
	char myLine[255];
	strcpy(myLine, line);

	//parse the command
	pch = strtok(myLine, " \n");
	if(!pch)
		goto UNKNOWN;	//eg. strtok("\n", " \n") will return null from strtok, goto unknown command
	strcpy(command, pch);
	if(!strcmp(command, "addStudy"))
		item->type = STUDY;
	else if(!strcmp(command, "addAssignment"))
		item->type = ASSIGNMENT;
	else if(!strcmp(command, "addProject"))
		item->type = PROJECT;
	else if(!strcmp(command, "addGathering"))
		item->type = GATHERING;
	else if(!strcmp(command, "addBatch"))
	{
		pch = strtok(NULL, " \n");
		char filename[255];
		strcpy(filename, pch);
		FILE *f = fopen(filename+1, "r");	//offset +1 to remove the '-'
		if(!f)
		{
			fprintf(stderr, "Failed to open file %s.\n", filename);
			exit(EXIT_FAILURE);
		}
		inputLoop(f);
		return;
	}
	else if(!strcmp(command, "printSchd"))
	{
		pch = strtok(NULL, " \n");
		char algorithmStr[30];
		strcpy(algorithmStr, pch);
		HandleSchedule(algorithmStr);
		return;
	}
	else if(!strcmp(command, "endProgram"))
	{
		printf("Received end program command.\n");
		exit(EXIT_SUCCESS);
	}
	else
	{
		UNKNOWN:
		printf("Unknown command: %s", line);
		return;
	}

	pch = strtok(NULL, " \n");
	strcpy(caller, pch+1);

	pch = strtok(NULL, " \n");
	sscanf(pch, "%d-%d-%d", &year, &month, &day);
	
	pch = strtok(NULL, " \n");
	sscanf(pch, "%d:%d", &hour, &minutes);
	
	pch = strtok(NULL, " \n"); duration = atof(pch);

	while(1)
	{
		pch = strtok(NULL, " \n");
		if(!pch)
			break;
		int id = GetUserID(pch);
		item->callee_id[callee_count++] = id;
	}

	item->caller_id = GetUserID(caller);
	//time
	struct tm timeinfo, timeinfo_tmp;
	memset(&timeinfo, 0, sizeof(timeinfo));
	timeinfo.tm_isdst = -1;
	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;
	//start time
	//convert ot half hour base
	timeinfo.tm_hour = hour;
	if(minutes>=0 && minutes <= 30)
		timeinfo.tm_min = 0;
	else
		timeinfo.tm_min = 30;

	timeinfo_tmp = timeinfo;	//because mktime could modify the value
	item->start = mktime(&timeinfo_tmp);
	//convert duration to end time
	double _;
	double fractional = modf(duration, &_);
	minutes += fractional*60;
	hour = hour+(int)duration;
	if(minutes>=60)
		hour++;
	minutes %= 60;

	timeinfo.tm_hour = hour;
	if(minutes>0 && minutes <= 30)
		timeinfo.tm_min = 0;
	else
		timeinfo.tm_min = 30;
	item->end = mktime(&timeinfo);

	item->id = inputList->count;
	AddAppointment(inputList, item);
	printf("-> [Pending]\n");
}


void inputLoop(FILE *stream)
{
	const int MAX_CHAR = 255;
	char line[MAX_CHAR];
	char *return_val;
	while(1)
	{
		return_val = fgets(line, MAX_CHAR, stream);
		if(!return_val)
		{
			if(feof(stream))
			{
				printf("Received EOF.\n");
				return;
			}
			else
			{
				fprintf(stderr, "IO error, existing program.\n");
				exit(EXIT_FAILURE);
			}
		}
		HandleInput(line);
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

	struct Appointment *b = CreateAppointment();
	b->type = GATHERING;
	printf("Priority: %d\n", CompareAppointmentPriority(a,b));
	printf("ASSIGNMENT : %d\n", ASSIGNMENT);
	printf("GATHERING : %d\n", GATHERING);
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
	a->start = timer+18000;
	a->end = timer+18000;
	AddAppointment(list, a);	//<<<<<<Testing
	PrintAppointmentList(list);	//<<<<<<Testing
	RemoveItemFromList(list, a); //<<<<<<Testing
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
void test4()
{
	printf("======================\n");
	printf("TEST 4:\n");
	HandleInput("addStudy -alice 2015-08-04 13:00 3.0");
	HandleInput("addAssignment -bob 2015-08-04 13:00 3.0");
	HandleInput("addGathering -charlie 2015-08-04 17:00 2.5");
	HandleInput("addAssignment -alice 2015-08-04 14:00 3.0");
	HandleInput("addStudy -bob 2015-08-04 14:00 3.0");
	HandleInput("addStudy -charlie 2015-08-04 14:00 3.0");
	PrintAppointmentList(inputList);
}
void test5()
{
	printf("======================\n");
	printf("TEST 5:\n");
	HandleInput("addProject -alice 2015-08-04 17:00 2 bob");
	HandleInput("addGathering -bob 2015-08-04 14:00 3.0 alice charlie");
	PrintAppointmentList(inputList);
}
void test6()
{
	printf("======================\n");
	printf("TEST 6:\n");
	Schedual_FCFS(inputList);
	PrintAllUser();
}
void test7()
{
	printf("======================\n");
	printf("TEST 7:\n");
	//reset user list
	for (int i=0; i<NumOfUser; i++)
	{
		user[i].accepted = CreateAppointmentList();
		user[i].rejected = CreateAppointmentList();
	}
	Schedual_PRIO(inputList);
	PrintAllUser();
}
void testAll()
{
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	printf("======END OF DEBUG MESSAGE======\n");
	printf("================================\n");
}

int main(int argc, char* argv[])
{
	if (argc < 4 || argc > 11)
	{
		fprintf(stderr, "Error: The number of users should between 3 and 10.\n");
		return EXIT_FAILURE;
	}
	NumOfUser = argc - 1;
	//Initialize each user in struct user[];
	for (int i=0; i<NumOfUser; i++)
	{
		strcpy(user[i].username, argv[i+1]);
		user[i].username[0] = toupper(user[i].username[0]);
		user[i].accepted = CreateAppointmentList();
		user[i].rejected = CreateAppointmentList();
	}
	inputList = CreateAppointmentList();

	//testAll();
	inputLoop(stdin);
	return EXIT_SUCCESS;
}