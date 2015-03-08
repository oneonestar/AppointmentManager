#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "appointment_list.h"
#include "user.h"

struct AppointmentList *inputList;

void HandleInput(const char *line);
void HandleSchedule(const char *line);

void HandleSchedule(const char *line)
{
	printf("HandleingScheduling: ");
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
	char *myLine = (char*)malloc(strlen(line));
	strcpy(myLine, line);

	//parse the command
	pch = strtok(myLine, " ");
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
		return;	//TODO: call the batch handling function
	else if(!strcmp(command, "printSchd"))
	{
		HandleSchedule(line);
		return;
	}
	else if(!strcmp(command, "endProgram"))
	{
		printf("Received end program command.\n");
		exit(EXIT_SUCCESS);
	}

	pch = strtok(NULL, " ");
	strcpy(caller, pch+1);

	pch = strtok(NULL, " ");
	sscanf(pch, "%d-%d-%d", &year, &month, &day);
	
	pch = strtok(NULL, " ");
	sscanf(pch, "%d:%d", &hour, &minutes);
	
	pch = strtok(NULL, " "); duration = atof(pch);

	while(1)
	{
		pch = strtok(NULL, " ");
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

	AddAppointment(inputList, item);
	free(myLine);
	/*
	int _;
	timeinfo.tm_hour = hour + (int)duration;
	timeinfo.tm_min = minutes + modf(duration, &_)*60;
	item->end = mktime(&timeinfo);
	*/
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
void test4()
{
	printf("======================\n");
	printf("TEST 4:\n");
	HandleInput("addStudy -alice 2015-08-04 13:00 3.0");
	HandleInput("addStudy -bob 2015-08-04 13:00 3.0");
	HandleInput("addStudy -charlie 2015-08-04 13:00 3.0");
	HandleInput("addAssignment -alice 2015-08-04 14:00 3.0");
	HandleInput("addAssignment -bob 2015-08-04 14:00 3.0");
	HandleInput("addAssignment -charlie 2015-08-04 14:00 3.0");
	PrintAppointmentList(inputList);
}
void test5()
{
	printf("======================\n");
	printf("TEST 5:\n");
	HandleInput("addProject -alice 2015-08-04 13:00 3.0 bob");
	HandleInput("addGathering -bob 2015-08-04 14:00 3.0 alice charlie");
	PrintAppointmentList(inputList);
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
		strcpy(user[i].username, argv[i+1]);
		user[i].username[0] = toupper(user[i].username[0]);
	}
	inputList = CreateAppointmentList();

	test1();
	test2();
	test3();
	test4();
	test5();
	return EXIT_SUCCESS;
}