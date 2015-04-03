/**
* @file scheduler.h
* @brief Secheduling algorithms
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

#ifndef SCHEDULER
#define SCHEDULER

#include "user.h"
#include <time.h>

struct Summary
{
	int total_accepted;
	int total_rejected;
	int accepted[USER_NUMBER];
	int rejected[USER_NUMBER];
	int empty_timeslot[USER_NUMBER];
	time_t start;
	time_t end;
};
/**
 * @brief First come first served. The order is folloing the input order.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
struct Summary* Schedual_FCFS(struct AppointmentList *inputList);

/**
 * @brief Priority. The order is folloing the pre-defined priority.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
struct Summary* Schedual_PRIO(struct AppointmentList *inputList);

/**
 * @brief Optimized. Bonus part, reschedule those rejected appointments.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
struct Summary* Schedual_OPTI(struct AppointmentList *inputList);

/**
 * @brief Print out the summary about the scheduling
 */
void PrintSummary(struct Summary *summary);

#endif