#include <string.h>
#include <ctype.h>

#include "user.h"

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
