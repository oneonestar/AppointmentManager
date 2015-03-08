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