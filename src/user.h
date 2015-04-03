/**
* @file user.h
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

/**
 * @brief Print the accepted list for user.
 */
void PrintAccepted(const struct User *user);

/**
 * @brief Print the rejected list for user.
 */
void PrintRejected(const struct User *user);

/**
 * @brief Print the accepted & reject list for all users.
 */
void PrintAllUser();

#endif