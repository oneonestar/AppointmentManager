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
void AddAppointment(struct AppointmentList *list, const struct Appointment newItem);

/**
 * @brief Insert a copy of the appointment into the sorted appointment list.
 *        Using the start time then end time as the sorting condition.
 * @param[out] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
void AddAppointmentOrdered(struct AppointmentList *list, const struct Appointment newItem);

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
 * @brief Return the user that have the same username.
 * @param username The username that are used to compare.
 * @return The pointer to the user structure that have the same username. Return NULL if user is not found.
 */
struct User* GetUser(const char *username);


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
}

struct Appointment* CreateAppointment()
{
	struct Appointment *item = (struct Appointment*)malloc(sizeof(struct Appointment));
	if(!item)
	{
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	item->next = NULL;
}

void AddAppointment(struct AppointmentList *list, const struct Appointment newItem)
{
	struct Appointment *item = CreateAppointment();
	*item = newItem;
	if(!list->head)	//if the list is empty
		list->head = item;
	else
		list->tail->next = item;
	list->tail = item;
	list->count++;
}

void AddAppointmentOrdered(struct AppointmentList *list, struct Appointment newItem)
{
	struct Appointment *item = CreateAppointment();
	*item = newItem;
	struct Appointment *ptr = list->head;
	//if the list is empty
	if(!ptr)
	{
		list->head = item;
		list->tail = item;
	}
	//if insert at the head
	if(difftime(ptr->start, newItem->start)>0)
	//find the insertion position
	while(!ptr->next)
	{
		if(difftime(ptr->start, newItem->start)>0)
			break;
		ptr = ptr->next;
	}
	list->count++;
}

int CompareAppointment(const struct Appointment *a, const struct Appointment *b)
{
	
}


struct User* GetUser(const char *username)
{
	for(int i=0; i<NumOfUser; i++)
		if (!strcmp(user[i].username, username))
			return &user[i];
	return NULL;
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
		strcpy(user[i].username, argv[i]);

	return EXIT_SUCCESS;
}