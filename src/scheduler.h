#ifndef SCHEDULER
#define SCHEDULER

/**
 * @brief First come first served. The order is folloing the input order.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
void Schedual_FCFS();


/**
 * @brief Priority. The order is folloing the pre-defined priority.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
void Schedual_PRIO();

/**
 * @brief Optimized. Bonus part, reschedule those rejected appointments.
 * The result will be putted into the each user's appointment lists (accept / reject).
 */
void Schedual_Opti();

#endif