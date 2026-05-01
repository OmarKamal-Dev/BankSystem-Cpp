#include <iostream>
#include <string>

#include "core/Types.h"
#include "core/Auth.h"
#include "data/Storage.h"
#include "ui/UserDashboard.h"
#include "ui/AdminDashboard.h"
#include "utils/Helpers.h"

using namespace std;

// ─── Session globals ─────────────────────
namespace Session
{
    sUser              currentUser;
    vector<sUser>      accounts;
}

// ─────────────────────────────────────────
//  Login Screen
// ─────────────────────────────────────────

void login()
{
    while (true)
    {
        loadSessionData();
        system("cls");

        cout << "\t\t  *************************************************************************************\n";
        cout << "\t\t  *****                                                                           *****\n";
        cout << "\t\t  *****           H E L L O !  W E L C O M E  T O  T H E  'Bank System'           *****\n";
        cout << "\t\t  *****            = = = = = = = = = = = = = = = = = = = = = = = = =              *****\n";
        cout << "\t\t  *****                      = = = = = = = = = = = = = =                          *****\n";
        cout << "\t\t  *****                                   B Y                                     *****\n";
        cout << "\t\t  *****                               Omar   Kamal                                *****\n";
        cout << "\t\t  *****                                                                           *****\n";
        cout << "\t\t  *************************************************************************************\n";
        cout << "\n\n\n";

        cout << "\t\t                                     Username\n\n";
        cout << "\t\t                                     ";
        string username = promptUsername();

        cout << "\n\n\t\t                                     Password\n\n";
        cout << "\t\t                                     ";
        string password = promptPassword(username);

        if (password.empty())
        {
            // Account locked after failed attempts
            return;
        }

        Session::currentUser = findUserByName(username);

        // Route to the correct dashboard
        if (Session::currentUser.accountType == Admin ||
            (Session::currentUser.permissions & PermAdminDashboard))
        {
            adminDashboard();
        }
        else
        {
            userDashboard();
        }
    }
}

// ─────────────────────────────────────────
//  Entry Point
// ─────────────────────────────────────────

int main()
{
    login();
    return 0;
}
