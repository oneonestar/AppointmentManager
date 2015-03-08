#ifndef APPOINTMENT_LIST
#define APPOINTMENT_LIST

#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "user.h"
/** 
 * @struct AppointmentType
 * @brief Store all the appointment type.
 */
enum AppointmentType
{
	STUDY = 0, ASSIGNMENT, PROJECT, GATHERING
};

/** For printing */
static const char *AppointmentTypeStr[] = {[STUDY] = "Study", [ASSIGNMENT] = "Assignment",
	[PROJECT] = "Project", [GATHERING] = "Gathering"};

struct Appointment
{
	enum AppointmentType type;
	int caller_id;
	int callee_id[10];
	time_t start;
	time_t end;
	int rescheduled;
	struct Appointment *prev;
	struct Appointment *next;
};	

struct AppointmentList
{
	int count;
	struct Appointment *head;
	struct Appointment *tail;
};


/**
 * @brief Create a appointment and init the value.
 */
struct Appointment* CreateAppointment();

/**
 * @brief Create a appointment list and init the value.
 */
struct AppointmentList* CreateAppointmentList();


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
 * @brief Check whether the the new appointment is conflict with the existing appointments that are already in the list.
 * @param[in] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
struct AppointmentList* IsConflictInList(const struct AppointmentList *list, const struct Appointment *item);

/**
 * @brief Check whether if two appointments have time conflict.
 */
int IsConflict(const struct Appointment *a, const struct Appointment *b);

/**
 * @brief Remove an item from the list. Items should be unique inside the list.
 * Delete if the two item have the same start time and end time.
 */
void RemoveItemFromList(const struct AppointmentList *list, const struct Appointment *item);

void RemoveListFromList(const struct AppointmentList *list, const struct Appointment *item);
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
 * @brief Print out the appointment.
 * @param item Appointment to be printed.
 */
void PrintAppointment(const struct Appointment *item);

/**
 * @brief Print out the appointment list.
 * @param list Appointment list to be printed.
 */
void PrintAppointmentList(const struct AppointmentList *list);

#endif