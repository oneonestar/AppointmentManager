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
struct Summary* Schedual_Opti(struct AppointmentList *inputList);

void PrintSummary(struct Summary *summary);

#endif