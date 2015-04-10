/**
* @file appointment_list.h
* @brief Handling the appointments and appointment list.
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
	ASSIGNMENT = 0, PROJECT, STUDY, GATHERING
};

/**
 * @struct Appointment
 * @brief Store a appointment record.
 */
struct Appointment
{
	enum AppointmentType type;
	int id;
	int caller_id;
	int callee_id[10];
	time_t start;
	time_t end;
	int is_accepted;
	int rescheduled;
	char reason[50];
	struct Appointment *prev;
	struct Appointment *next;
};

/**
 * @struct AppointmentList
 * @brief A double-linked list for appointment record.
 */
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
 * @brief Insert a copy of the appointment into the end of the appointment list.
 * @param[out] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
void AddAppointmentFromList(struct AppointmentList *dst_list, const struct AppointmentList *src_list);

/**
 * @brief Insert a copy of the appointment into the sorted appointment list.
 *        Using the start time then end time as the sorting condition.
 * @param[out] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
void AddAppointmentOrderedFromList(struct AppointmentList *dst_list, const struct AppointmentList *src_list);

/**
 * @brief Check whether the the new appointment is conflict with the existing appointments that are already in the list.
 * @param[in] list The destination appointment list.
 * @param[in] newItem The item that needs to add into the list.
 */
struct AppointmentList* ConflictInList(const struct AppointmentList *list, const struct Appointment *item);

/**
 * @brief Check whether if two appointments have time conflict.
 */
int IsConflict(const struct Appointment *a, const struct Appointment *b);

/**
 * @brief Check whether if the appointment item have conflict with the list.
 */
int IsConflictInList(const struct AppointmentList *list, const struct Appointment *item);

/**
 * @brief Remove an item from the list. Items should be unique inside the list.
 * Delete if the two item have the same id.
 */
int RemoveItemFromList(struct AppointmentList *list, const struct Appointment *item);

/**
 * @brief Remove a list of items from the list. Items should be unique inside the list.
 * Delete if the two item have the same id.
 */
void RemoveListFromList(struct AppointmentList *ori_list, const struct AppointmentList *del_list);

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
 * @brief Compare the appointment by it's priority.
 * @param[in] a Appointment to be compared.
 * @param[in] b Appointment to be compared.
 * @retval <0 a is before b
 * @retval 0 a is equal to b
 * @retval >0 a is after b
 */
int CompareAppointmentPriority(const struct Appointment *a, const struct Appointment *b);

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

/**
 * @brief Return the appointment that match the id in the list.
 */
struct Appointment* GetAppointmentById(const struct AppointmentList *list, int id);

/**
 * @brief Set the reject reason.
 */
void SetReasonForList(struct AppointmentList *list, const char *reason);

#endif