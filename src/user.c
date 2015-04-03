/**
* @file user.c
* @brief Handling each users
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

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "user.h"
#include "appointment_list.h"

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

int GetUserID(const char *username)
{
	for(int i=0; i<NumOfUser; i++)
		if (!strcicmp(user[i].username, username))
			return i;
	return -1;
}



void PrintAccepted(const struct User *user)
{
	printf("%s, you have %d appointments.\n", user->username, user->accepted->count);
	PrintAppointmentList(user->accepted);
}



void PrintRejected(const struct User *user)
{
	printf("%s, you have %d appointments rejected.\n", user->username, user->rejected->count);
	PrintAppointmentList(user->rejected);
}


void PrintAllUser()
{
	printf("***Appointment Schedule – ACCEPTED ***\n\n");
	for(int i=0; i<NumOfUser; i++)
	{
		PrintAccepted(&user[i]);
	}
	printf("   -End-\n");
	printf("===========================================================================\n");
	printf("***Appointment Schedule – REJECTED ***\n\n");
	for(int i=0; i<NumOfUser; i++)
	{\
		PrintRejected(&user[i]);
	}
	printf("   -End-\n");
	printf("===========================================================================\n");
}