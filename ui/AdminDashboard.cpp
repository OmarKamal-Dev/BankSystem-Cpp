#include <iostream>
#include <iomanip>
#include <string>
#include "AdminDashboard.h"
#include "../core/Types.h"
#include "../core/Auth.h"
#include "../utils/Helpers.h"
#include "../data/Storage.h"

using namespace std;

// ─────────────────────────────────────────
//  Internal helper: ask permissions
// ─────────────────────────────────────────

static short promptPermissions()
{
    short perms = 0;
    string answer;

    cout << "\t\t                Give Full Access? (y/n) : ";
    answer = readString();
    if (answer == "y" || answer == "Y")
        return -1;   // all bits set

    auto askPerm = [&](const string& label, enPermission flag)
    {
        cout << "\t\t                Allow " << label << "? (y/n) : ";
        answer = readString();
        if (answer == "y" || answer == "Y")
            perms |= flag;
    };

    askPerm("Deposit",         PermDeposit);
    askPerm("Withdraw",        PermWithdraw);
    askPerm("Transfer",        PermTransfer);
    askPerm("Borrow",          PermBorrow);
    askPerm("Update Profile",  PermUpdateProfile);
    askPerm("Admin Dashboard", PermAdminDashboard);

    return perms;
}

// ─────────────────────────────────────────
//  Users List
// ─────────────────────────────────────────

void showUsersList()
{
    vector<sUser> users = loadAllUsers();

    system("cls");
    displayPanel("U S E R S   L I S T", 107);
    cout << "\n\n";

    cout << "\t\t " << setfill('*') << setw(107) << "" << "\n";
    cout << setfill(' ');
    cout << "\t\t "
         << "* " << left << setw(12) << "USER ID"
         << "| " << left << setw(18) << "Username"
         << "| " << left << setw(18) << "Password"
         << "| " << left << setw(14) << "Account Type"
         << "| " << left << setw(12) << "Permissions"
         << "| " << left << setw(20) << "Balance"
         << "*\n";

    for (size_t i = 0; i < users.size(); i++)
    {
        const sUser& u = users[i];
        cout << "\t\t " << setfill('*') << setw(107) << "" << "\n";
        cout << setfill(' ');
        cout << "\t\t "
             << "* " << left << setw(12) << u.userId
             << "| " << left << setw(18) << u.username
             << "| " << left << setw(18) << u.password
             << "| " << left << setw(14) << (u.accountType == Admin ? "Admin" : "User")
             << "| " << left << setw(12) << u.permissions
             << "| " << left << setw(20) << formatCurrency(u.balance)
             << "*\n";
    }
    cout << "\t\t " << setfill('*') << setw(107) << "" << "\n";

    cout << "\n\n\t\t      Press Enter to return...";
    cin.ignore();
    cin.get();
}

// ─────────────────────────────────────────
//  Add New User
// ─────────────────────────────────────────

void addNewUser()
{
    string tryAgain = "Y";

    while (tryAgain == "Y" || tryAgain == "y")
    {
        vector<sUser> users = loadAllUsers();

        system("cls");
        displayPanel("A D D   N E W   U S E R", 102);
        cout << "\n\n";

        // Username (must be unique)
        string newUsername;
        do
        {
            cout << "\t\t                           Enter Username\n\n";
            cout << "\t\t                                     ";
            newUsername = readString();

            if (!isUsernameAvailable(newUsername, users))
            {
                cout << "\t\t                     Username already exists. Please try again.\n\n";
            }
        }
        while (!isUsernameAvailable(newUsername, users));

        // Password
        cout << "\n\n\t\t                                     Password\n\n";
        cout << "\t\t                                     ";
        string password = readString();

        // Permissions
        short perms = promptPermissions();

        // Opening balance
        cout << "\n\n\t\t                                            Opening Balance : ";
        double balance = readValidDouble();

        // Build user record
        sUser newUser;
        newUser.userId      = generateUniqueUserId();
        newUser.username    = newUsername;
        newUser.password    = password;
        newUser.permissions = perms;
        newUser.accountType = (perms & PermAdminDashboard) ? Admin : User;
        newUser.balance     = balance;

        users.push_back(newUser);
        saveAllUsers(users);

        showSuccessScreen("U S E R   A D D E D   S U C C E S S F U L L Y");
        cout << "\n\n\n\n\t\t                Add Another User? (y/n) : ";
        tryAgain = readString();
    }
}

// ─────────────────────────────────────────
//  Delete User
// ─────────────────────────────────────────

void deleteUser()
{
    string tryAgain = "Y";

    while (tryAgain == "Y" || tryAgain == "y")
    {
        vector<sUser> users = loadAllUsers();

        system("cls");
        displayPanel("D E L E T E   U S E R", 102);
        cout << "\n\n";

        string targetUsername;
        bool   found = false;

        do
        {
            cout << "\t\t                           Enter Username to Delete\n\n";
            cout << "\t\t                                     ";
            targetUsername = readString();

            found = !isUsernameAvailable(targetUsername, users);   // false means username exists
            if (!found)
                cout << "\t\t                     Username not found. Please try again.\n\n";
        }
        while (!found);

        int index = findUserIndex(targetUsername, users);

        cout << "\n\n\t\t                    Confirm deletion of \"" << targetUsername << "\"? (y/n) : ";
        string confirm = readString();

        if (confirm == "Y" || confirm == "y")
        {
            users.erase(users.begin() + index);
            saveAllUsers(users);
            showSuccessScreen("U S E R   D E L E T E D   S U C C E S S F U L L Y");
        }
        else
        {
            showSuccessScreen("N O T H I N G   W A S   D E L E T E D");
            return;
        }

        cout << "\n\n\n\n\t\t                Delete Another User? (y/n) : ";
        tryAgain = readString();
    }
}

// ─────────────────────────────────────────
//  Update User
// ─────────────────────────────────────────

void updateUser()
{
    string tryAgain = "Y";

    while (tryAgain == "Y" || tryAgain == "y")
    {
        vector<sUser> users = loadAllUsers();

        system("cls");
        displayPanel("U P D A T E   U S E R", 102);
        cout << "\n\n";

        string targetUsername;
        bool   found = false;

        do
        {
            cout << "\t\t                           Enter Username to Update\n\n";
            cout << "\t\t                                     ";
            targetUsername = readString();

            found = !isUsernameAvailable(targetUsername, users);
            if (!found)
                cout << "\t\t                     Username not found. Please try again.\n\n";
        }
        while (!found);

        int index = findUserIndex(targetUsername, users);

        system("cls");
        printUserCard(users[index]);
        cout << "\n\n";

        // New username
        cout << "\t\t                Enter New Username (leave blank to keep current) : ";
        string newUsername = readString();
        if (!newUsername.empty() && newUsername != users[index].username)
        {
            while (!isUsernameAvailable(newUsername, users))
            {
                cout << "\t\t                Username already taken. Try another: ";
                newUsername = readString();
            }
            users[index].username = newUsername;
        }

        // New password
        cout << "\t\t                Enter New Password  (leave blank to keep current) : ";
        string newPassword = readString();
        if (!newPassword.empty())
            users[index].password = newPassword;

        // Permissions
        short perms = promptPermissions();
        users[index].permissions = perms;
        users[index].accountType = (perms & PermAdminDashboard) ? Admin : User;

        saveAllUsers(users);
        showSuccessScreen("U S E R   U P D A T E D   S U C C E S S F U L L Y");

        cout << "\n\n\n\n\t\t                Update Another User? (y/n) : ";
        tryAgain = readString();
    }
}

// ─────────────────────────────────────────
//  Find User
// ─────────────────────────────────────────

void findUser()
{
    string tryAgain = "Y";

    while (tryAgain == "Y" || tryAgain == "y")
    {
        vector<sUser> users = loadAllUsers();

        system("cls");
        displayPanel("F I N D   U S E R", 102);
        cout << "\n\n";

        string targetUsername;
        bool   found = false;

        do
        {
            cout << "\t\t                           Enter Username to Find\n\n";
            cout << "\t\t                                     ";
            targetUsername = readString();

            found = !isUsernameAvailable(targetUsername, users);
            if (!found)
                cout << "\t\t                     Username not found. Please try again.\n\n";
        }
        while (!found);

        int index = findUserIndex(targetUsername, users);

        system("cls");
        printUserCard(users[index]);

        cout << "\n\n\n\n\n\n\t\t                Find Another User? (y/n) : ";
        tryAgain = readString();
    }
}

// ─────────────────────────────────────────
//  All Transactions History (admin view)
// ─────────────────────────────────────────

void showAllTransactionHistory()
{
    vector<sTransaction> transactions = loadAllTransactions();

    if (transactions.empty())
    {
        showSuccessScreen("T R A N S A C T I O N   H I S T O R Y   I S   E M P T Y");
        return;
    }

    system("cls");
    displayPanel("F U L L   T R A N S A C T I O N   H I S T O R Y", 160);
    cout << "\n\n";

    cout << "\t\t " << setfill('*') << setw(160) << "" << "\n";
    cout << setfill(' ');
    cout << "\t\t "
         << "* " << left << setw(12) << "TX ID"
         << "| " << left << setw(14) << "TYPE"
         << "| " << left << setw(16) << "PERFORMED BY"
         << "| " << left << setw(16) << "FROM ACCOUNT"
         << "| " << left << setw(16) << "DESTINATION"
         << "| " << left << setw(16) << "BAL. BEFORE"
         << "| " << left << setw(14) << "AMOUNT"
         << "| " << left << setw(16) << "BAL. AFTER"
         << "| " << left << setw(21) << "DATE & TIME"
         << "*\n";

    for (size_t i = 0; i < transactions.size(); i++)
    {
        const sTransaction& tx = transactions[i];

        sUser  sourceUser = getUserById(tx.accountId);
        string sourceLabel = sourceUser.username.empty() ? tx.accountId : sourceUser.username;

        sUser  destUser  = getUserById(tx.targetAccountId);
        string destLabel = (tx.targetAccountId == "0000") ? "BANK"
                         : (destUser.username.empty() ? tx.targetAccountId : destUser.username);

        cout << "\t\t " << setfill('*') << setw(160) << "" << "\n";
        cout << setfill(' ');
        cout << "\t\t "
             << "* " << left << setw(12) << tx.transactionId
             << "| " << left << setw(14) << getTransactionTypeLabel(tx.type)
             << "| " << left << setw(16) << tx.performedByUsername
             << "| " << left << setw(16) << sourceLabel
             << "| " << left << setw(16) << destLabel
             << "| " << left << setw(16) << formatCurrency(tx.balanceBefore)
             << "| " << left << setw(14) << formatCurrency(tx.amount)
             << "| " << left << setw(16) << formatCurrency(tx.balanceAfter)
             << "| " << left << setw(21) << tx.dateTime
             << "*\n";
    }
    cout << "\t\t " << setfill('*') << setw(160) << "" << "\n";

    cout << "\n\n\t\t      Press Enter to return...";
    cin.ignore();
    cin.get();
}

// ─────────────────────────────────────────
//  Admin Dashboard
// ─────────────────────────────────────────

void adminDashboard()
{
    while (true)
    {
        system("cls");
        displayPanel("A D M I N   D A S H B O A R D");

        cout << "\t\t                         [1] Users List\n\n";
        cout << "\t\t                         [2] Add New User\n\n";
        cout << "\t\t                         [3] Delete User\n\n";
        cout << "\t\t                         [4] Update User\n\n";
        cout << "\t\t                         [5] Find User\n\n";
        cout << "\t\t                         [6] Transaction History\n\n";
        cout << "\t\t                         [0] Return to Main Menu\n\n\n";
        cout << "\t\t                                   Enter Your Option : ";

        int choice = readValidIntInRange(0, 6);

        switch (choice)
        {
        case 1: showUsersList();             break;
        case 2: addNewUser();                break;
        case 3: deleteUser();                break;
        case 4: updateUser();                break;
        case 5: findUser();                  break;
        case 6: showAllTransactionHistory(); break;
        case 0: return;
        default: break;
        }
    }
}
