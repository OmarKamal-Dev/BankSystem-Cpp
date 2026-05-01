#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include "UserDashboard.h"
#include "../core/Types.h"
#include "../core/Auth.h"
#include "../utils/Helpers.h"
#include "../data/Storage.h"

using namespace std;

// ─────────────────────────────────────────
//  Show Balance
// ─────────────────────────────────────────

void showBalance()
{
    system("cls");
    cout << "\n\n\n\n\n\n";
    displayPanel("Y O U R    B A L A N C E    I S :  " + formatCurrency(Session::currentUser.balance));

    cout << "\n\n\t\t      Press Enter to return...";
    cin.ignore();
    cin.get();
}

// ─────────────────────────────────────────
//  Deposit
// ─────────────────────────────────────────

void depositMoney()
{
    system("cls");
    displayPanel("D E P O S I T    P A G E");
    cout << "\n\n\n";
    cout << "\t\t       H O W   M U C H   M O N E Y   D O   Y O U   W A N T   T O   D E P O S I T ?\n\n\n";
    cout << "\t\t                                     ";

    double amount = readValidDouble();
    if (amount == 0) return;

    recordTransaction(amount, Deposit, "0000");
    applyDeposit(amount);
    showSuccessScreen("O P E R A T I O N   C O M P L E T E D   S U C C E S S F U L L Y !");
}

// ─────────────────────────────────────────
//  Withdraw
// ─────────────────────────────────────────

void withdrawMoney()
{
    if (Session::currentUser.balance <= 0)
    {
        showSuccessScreen("Y O U   H A V E   N O   F U N D S   I N   Y O U R   A C C O U N T");
        return;
    }

    system("cls");
    displayPanel("W I T H D R A W    P A G E");
    cout << "\n\n\n";
    cout << "\t\t       H O W   M U C H   D O   Y O U   W A N T   T O   W I T H D R A W ?\n\n\n";
    cout << "\t\t                                     ";

    double amount = readValidDouble();
    if (amount == 0) return;

    while (Session::currentUser.balance < amount)
    {
        cout << "\t\t                   I N S U F F I C I E N T   F U N D S !   T R Y   A G A I N\n\n";
        cout << "\t\t       H O W   M U C H   D O   Y O U   W A N T   T O   W I T H D R A W ?\n\n\n";
        cout << "\t\t                                     ";
        amount = readValidDouble();
        if (amount == 0) return;
    }

    recordTransaction(amount, Withdraw, "0000");
    applyWithdraw(amount);
    showSuccessScreen("O P E R A T I O N   C O M P L E T E D   S U C C E S S F U L L Y !");
}

// ─────────────────────────────────────────
//  Transfer
// ─────────────────────────────────────────

void transferMoney()
{
    system("cls");
    displayPanel("T R A N S F E R    P A G E");
    cout << "\n\n\n";
    cout << "\t\t                E N T E R   R E C I P I E N T   U S E R N A M E\n\n\n";
    cout << "\t\t                                     ";

    string recipient = promptUsername();

    // Prevent self-transfer
    while (recipient == Session::currentUser.username)
    {
        cout << "\t\t                   You cannot transfer to yourself. Try again.\n";
        cout << "\t\t                                     ";
        recipient = promptUsername();
    }

    cout << "\n\n\n";
    cout << "\t\t       H O W   M U C H   D O   Y O U   W A N T   T O   S E N D ?\n\n\n";
    cout << "\t\t                                     ";

    double amount = readValidDouble();
    if (amount == 0) return;

    while (Session::currentUser.balance < amount)
    {
        cout << "\t\t       I N S U F F I C I E N T   F U N D S !   T R Y   A G A I N\n\n";
        cout << "\t\t       H O W   M U C H   D O   Y O U   W A N T   T O   S E N D ?\n\n\n";
        cout << "\t\t                                     ";
        amount = readValidDouble();
        if (amount == 0) return;
    }

    sUser recipientUser = findUserByName(recipient);

    recordTransaction(amount, Transfer, recipientUser.userId);
    applyTransfer(amount, recipientUser.userId);
    showSuccessScreen("O P E R A T I O N   C O M P L E T E D   S U C C E S S F U L L Y !");
}

// ─────────────────────────────────────────
//  Borrow
// ─────────────────────────────────────────

void borrowMoney()
{
    system("cls");
    displayPanel("B O R R O W    P A G E");
    cout << "\n\n\n";
    cout << "\t\t       H O W   M U C H   M O N E Y   D O   Y O U   W A N T   T O   B O R R O W ?\n\n\n";
    cout << "\t\t                                     ";

    double amount = readValidDouble();
    if (amount == 0) return;

    const double maxDebt = 10000.0;
    while (Session::currentUser.balance - amount < -maxDebt)
    {
        cout << "\t\t       S O R R Y ,   M A X I M U M   D E B T   L I M I T   I S   1 0 , 0 0 0\n\n";
        cout << "\t\t       H O W   M U C H   D O   Y O U   W A N T   T O   B O R R O W ?\n\n\n";
        cout << "\t\t                                     ";
        amount = readValidDouble();
        if (amount == 0) return;
    }

    recordTransaction(amount, Borrow, "0000");
    applyBorrow(amount);
    showSuccessScreen("O P E R A T I O N   C O M P L E T E D   S U C C E S S F U L L Y !");
}

// ─────────────────────────────────────────
//  Transaction History (user view)
// ─────────────────────────────────────────

void showUserTransactionHistory()
{
    vector<sTransaction> transactions = loadTransactionsByUser(Session::currentUser);

    if (transactions.empty())
    {
        showSuccessScreen("T R A N S A C T I O N   H I S T O R Y   I S   E M P T Y");
        return;
    }

    system("cls");
    displayPanel("Y O U R   R E C E N T   T R A N S A C T I O N S", 144);
    cout << "\n\n";

    cout << "\t\t " << setfill('*') << setw(144) << "" << "\n";
    cout << setfill(' ');
    cout << "\t\t "
         << "* " << left << setw(12) << "TX ID"
         << "| " << left << setw(12) << "TYPE"
         << "| " << left << setw(18) << "FROM"
         << "| " << left << setw(18) << "DESTINATION"
         << "| " << left << setw(16) << "BAL. BEFORE"
         << "| " << left << setw(14) << "AMOUNT"
         << "| " << left << setw(16) << "BAL. AFTER"
         << "| " << left << setw(21) << "DATE & TIME"
         << "*\n";

    for (size_t i = 0; i < transactions.size(); i++)
    {
        const sTransaction& tx = transactions[i];

        string fromUser = tx.performedByUsername;
        string toUser   = (tx.targetAccountId == "0000") ? "BANK"
                        : getUserById(tx.targetAccountId).username;

        cout << "\t\t " << setfill('*') << setw(144) << "" << "\n";
        cout << setfill(' ');
        cout << "\t\t "
             << "* " << left << setw(12) << tx.transactionId
             << "| " << left << setw(12) << getTransactionTypeLabel(tx.type)
             << "| " << left << setw(18) << fromUser
             << "| " << left << setw(18) << toUser
             << "| " << left << setw(16) << formatCurrency(tx.balanceBefore)
             << "| " << left << setw(14) << formatCurrency(tx.amount)
             << "| " << left << setw(16) << formatCurrency(tx.balanceAfter)
             << "| " << left << setw(21) << tx.dateTime
             << "*\n";
    }
    cout << "\t\t " << setfill('*') << setw(144) << "" << "\n";

    cout << "\n\n\t\t      Press Enter to return...";
    cin.ignore();
    cin.get();
}

// ─────────────────────────────────────────
//  Update User Profile (name & password only)
// ─────────────────────────────────────────

void updateUserProfile()
{
    system("cls");
    displayPanel("U P D A T E   Y O U R   P R O F I L E");
    cout << "\n\n";
    cout << "\t\t                         [1] Change Username\n\n";
    cout << "\t\t                         [2] Change Password\n\n";
    cout << "\t\t                         [0] Cancel\n\n\n";
    cout << "\t\t                                   Enter Your Option : ";

    int choice = readValidIntInRange(0, 2);
    if (choice == 0) return;

    vector<sUser> users = loadAllUsers();
    int index = findUserIndex(Session::currentUser.username, users);
    if (index == -1) return;

    if (choice == 1)
    {
        cout << "\n\t\t                Enter New Username : ";
        string newUsername = readString();

        // Make sure it's not already taken
        while (!isUsernameAvailable(newUsername, users))
        {
            cout << "\t\t                Username already exists. Try another: ";
            newUsername = readString();
        }

        users[index].username          = newUsername;
        Session::currentUser.username  = newUsername;
    }
    else if (choice == 2)
    {
        cout << "\n\t\t                Enter Current Password   : ";
        string current = readString();

        if (current != Session::currentUser.password)
        {
            showSuccessScreen("I N C O R R E C T   C U R R E N T   P A S S W O R D");
            return;
        }

        cout << "\t\t                Enter New Password       : ";
        string newPassword = readString();

        users[index].password          = newPassword;
        Session::currentUser.password  = newPassword;
    }

    saveAllUsers(users);
    showSuccessScreen("P R O F I L E   U P D A T E D   S U C C E S S F U L L Y !");
}

// ─────────────────────────────────────────
//  User Dashboard
// ─────────────────────────────────────────

void userDashboard()
{
    while (true)
    {
        system("cls");
        displayPanel(Session::currentUser.username + "'s Dashboard");

        short perms = Session::currentUser.permissions;

        // بيبني القايمة بس على الـ permissions الموجودة
        int option = 0;
        map<int, int> menuMap;   // رقم القايمة → الـ permission

        cout << "\n";
        if (perms & PermDeposit)
        {
            menuMap[++option] = PermDeposit;
            cout << "\t\t                         [" << option << "] Deposit Money\n\n";
        }
        if (perms & PermWithdraw)
        {
            menuMap[++option] = PermWithdraw;
            cout << "\t\t                         [" << option << "] Withdraw Money\n\n";
        }
        if (perms & PermTransfer)
        {
            menuMap[++option] = PermTransfer;
            cout << "\t\t                         [" << option << "] Transfer Money\n\n";
        }
        if (perms & PermBorrow)
        {
            menuMap[++option] = PermBorrow;
            cout << "\t\t                         [" << option << "] Borrow Money\n\n";
        }

        cout << "\t\t                         [" << ++option << "] Show Balance\n\n";
        int balanceOption = option;

        cout << "\t\t                         [" << ++option << "] Transaction History\n\n";
        int historyOption = option;

        if (perms & PermUpdateProfile)
        {
            cout << "\t\t                         [" << ++option << "] Update Profile\n\n";
        }
        int updateOption = (perms & PermUpdateProfile) ? option : -1;

        cout << "\t\t                         [0] Logout\n\n\n";
        cout << "\t\t                                   Enter Your Option : ";

        int choice = readValidIntInRange(0, option);
        if (choice == 0) return;

        if (choice == balanceOption)  showBalance();                
        if (choice == historyOption)  showUserTransactionHistory(); 
        if (choice == updateOption)   updateUserProfile();          

        if (menuMap.count(choice))
        {
            switch (menuMap[choice])
            {
            case PermDeposit:  depositMoney();  break;
            case PermWithdraw: withdrawMoney(); break;
            case PermTransfer: transferMoney(); break;
            case PermBorrow:   borrowMoney();   break;
            }
        }
    }
}
