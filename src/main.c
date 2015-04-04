/**
* @file main.c
* @brief Appointment Manager (AMR) main program. Also the input handling.
* @author oneonestar <oneonestar@gmail.com>
* @version 1.0
* @copyright 2015 
*
* @section LICENSE
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

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
	if(inputList->count == 0)
	{
		printf("Empty timetable.\n");
		return;
	}

#ifdef NO_FORK
	struct Summary *summary = NULL;
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
		summary = Schedual_OPTI(inputList);
	else
	{
		printf("Unknown scheduler.\n");
		return;
	}
	PrintAllUser();
	PrintSummary(summary);
#else
	struct Summary *summary;
	int fd[2];
	if (pipe(fd) < 0) {
		printf("Pipe creation error\n");
		exit(EXIT_FAILURE);
	}

	int ret=fork();
	if (ret < 0)
	{
		printf("error in fork!");
		exit(EXIT_FAILURE);
	}
	else if (ret == 0) {	//Child
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
			summary = Schedual_OPTI(inputList);
		else
		{
			printf("Unknown scheduler.\n");
			return;
		}
		PrintAllUser();
		if(write(fd[1], summary, sizeof(struct Summary)) < 0)
			printf("Oh dear, something went wrong with write()! %s\n", strerror(errno));
		_exit(EXIT_SUCCESS);
	}

	summary = (struct Summary *)malloc(sizeof(struct Summary));
	if(read(fd[0], summary, sizeof(struct Summary)) < 0)
			printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
	wait(NULL);
	PrintSummary(summary);
#endif
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
			return;
			// exit(EXIT_FAILURE);
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
		printf("Unknown command: %s\n", line);
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
		printf("Please enter appointment:\n");
		return_val = fgets(line, MAX_CHAR, stream);
		if(!return_val)
		{
			// sleep(10);
			if(feof(stream))
			{
				printf("Received EOF.\n");
				clearerr(stdin);
				//Allow ./AMR a b < input.txt, continue to read user input after EOF from stdin.
				//WARNING: not portable, only works in linux!
				if (!freopen("/dev/tty", "rw", stdin)) {
					perror("freopen");
					exit(EXIT_FAILURE);
				}
				continue;
			}
			else
			{
				fprintf(stderr, "IO error, existing program.\n");
				return;
				// exit(EXIT_FAILURE);
			}
		}
		HandleInput(line);
	}
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
		if(GetUserID(argv[i+1]) != -1)
		{
			printf("Duplicate names of users!\n");
			exit(EXIT_FAILURE);
		}
		strcpy(user[i].username, argv[i+1]);
		user[i].username[0] = toupper(user[i].username[0]);
		user[i].accepted = CreateAppointmentList();
		user[i].rejected = CreateAppointmentList();
	}
	inputList = CreateAppointmentList();

	printf("~~WELCOME TO AMR~~\n");
	inputLoop(stdin);
	return EXIT_SUCCESS;
}