#include <iostream>
#include "Auth.h"
#include "../utils/Helpers.h"

using namespace std;

bool isUsernameValid(const string& username, const vector<sUser>& users)
{
    for (const auto& user : users)
        if (user.username == username)
            return true;
    return false;
}

bool isPasswordCorrect(int userIndex, const string& password, const vector<sUser>& users)
{
    return users.at(userIndex).password == password;
}

int findUserIndex(const string& username, const vector<sUser>& users)
{
    for (int i = 0; i < (int)users.size(); i++)
        if (users[i].username == username)
            return i;
    return -1;
}

sUser findUserByName(const string& username)
{
    for (const auto& user : Session::accounts)
        if (user.username == username)
            return user;
    return sUser{};
}

string promptUsername()
{
    string username = readString();
    while (!isUsernameValid(username, Session::accounts))
    {
        cout << "\n\t\t                           \"" << username << "\" not found. Please try again.\n";
        cout << "\n\t\t                                     ";
        username = readString();
    }
    return username;
}

string promptPassword(const string& username)
{
    int userIndex = findUserIndex(username, Session::accounts);

    for (int attemptsLeft = 2; attemptsLeft >= 0; attemptsLeft--)
    {
        string password = readString();

        if (isPasswordCorrect(userIndex, password, Session::accounts))
            return password;

        if (attemptsLeft > 0)
        {
            cout << "\t\t                       Incorrect password. "
                 << attemptsLeft << " attempt(s) remaining.\n";
            cout << "\t\t                                        ";
        }
        else
        {
            cout << "\t\t                                Program is frozen for 5 minutes.\n";
        }
    }
    return "";   
}
