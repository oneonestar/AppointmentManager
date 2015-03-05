#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define USER_NUMBER 10
#define MAX_USERNAME 31

struct User
{
	char username[MAX_USERNAME];
	struct AppointmentList accepted;
	struct AppointmentList rejected;
} users[USER_NUMBER];

enum AppointmentType
{
	STUDY, ASSIGNMENT, PROJECT, GATHERING
};

struct Appointment
{
	enum AppointmentType type;
	int caller_id;
	int callee_id[10];
	time_t start;
	time_t end;
	struct Appointment *next;
};

struct AppointmentList
{
	int count;
	struct Appointment *head;
};

struct AppointmentList* CreateAppointmentList();
struct Appointment* CreateAppointment();

int CheckCollision(struct AppointmentList *list, struct Appointment newItem);
void AddAppointment(struct AppointmentList *list, struct Appointment newItem);

