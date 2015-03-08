#ifndef USER
#define USER
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
};

/**
 * Global variable storing the user data.
 */
struct User user[USER_NUMBER];

/**
 * Global variable storing current number of user.
 */
int NumOfUser;

/**
 * @brief Return the user id that have the same username.
 * @param username The username that are used to compare.
 * @return The user id that have the same username. Return -1 if user is not found.
 */
int GetUserID(const char *username);

#endif