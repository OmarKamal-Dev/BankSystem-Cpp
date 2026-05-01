#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "Storage.h"
#include "../utils/Helpers.h"

using namespace std;

// ─────────────────────────────────────────
//  File Constants
// ─────────────────────────────────────────

const string USERS_FILE        = "db/users.txt";
const string TRANSACTIONS_FILE = "db/transactions.txt";
const string FIELD_SEPARATOR   = "|";

// ─────────────────────────────────────────
//  Serialization / Deserialization
// ─────────────────────────────────────────

static sUser parseUserLine(const string& line)
{
    sUser          user;
    vector<string> fields = splitString(line, FIELD_SEPARATOR);

    if (fields.size() < 6) return user;   

    user.userId      = fields[0];
    user.username    = fields[1];
    user.password    = fields[2];
    user.accountType = (fields[3] == "Admin") ? Admin : User;
    user.permissions = static_cast<short>(stoi(fields[4]));
    user.balance     = stod(fields[5]);

    return user;
}

static string serializeUser(const sUser& user)
{
    return
        user.userId                                           + FIELD_SEPARATOR +
        user.username                                         + FIELD_SEPARATOR +
        user.password                                         + FIELD_SEPARATOR +
        (user.accountType == Admin ? "Admin" : "User")        + FIELD_SEPARATOR +
        to_string(user.permissions)                           + FIELD_SEPARATOR +
        to_string(user.balance);
}

static sTransaction parseTransactionLine(const string& line)
{
    sTransaction   tx;
    vector<string> fields = splitString(line, FIELD_SEPARATOR);

    if (fields.size() < 9) return tx;   

    tx.transactionId       = fields[0];
    tx.performedByUsername = fields[1];
    tx.accountId           = fields[2];
    tx.targetAccountId     = fields[3];
    tx.type                = static_cast<enTransactionType>(stoi(fields[4]));
    tx.balanceBefore       = stod(fields[5]);
    tx.amount              = stod(fields[6]);
    tx.balanceAfter        = stod(fields[7]);
    tx.dateTime            = fields[8];

    return tx;
}

static string serializeTransaction(const sTransaction& tx)
{
    return
        tx.transactionId       + FIELD_SEPARATOR +
        tx.performedByUsername + FIELD_SEPARATOR +
        tx.accountId           + FIELD_SEPARATOR +
        tx.targetAccountId     + FIELD_SEPARATOR +
        to_string(tx.type)          + FIELD_SEPARATOR +
        to_string(tx.balanceBefore) + FIELD_SEPARATOR +
        to_string(tx.amount)        + FIELD_SEPARATOR +
        to_string(tx.balanceAfter)  + FIELD_SEPARATOR +
        tx.dateTime;
}

// ─────────────────────────────────────────
//  Unique ID Generation (collision-safe)
// ─────────────────────────────────────────

string generateUniqueUserId()
{
    vector<sUser> existing = loadAllUsers();
    set<string>   usedIds;
    for (const auto& u : existing)
        usedIds.insert(u.userId);

    string id;
    do { id = generateUniqueId("U-"); }
    while (usedIds.count(id));

    return id;
}

string generateUniqueTransactionId()
{
    vector<sTransaction> existing = loadAllTransactions();
    set<string>          usedIds;
    for (const auto& tx : existing)
        usedIds.insert(tx.transactionId);

    string id;
    do { id = generateUniqueId("TX-"); }
    while (usedIds.count(id));

    return id;
}

// ─────────────────────────────────────────
//  Session
// ─────────────────────────────────────────

void loadSessionData()
{
    Session::accounts = loadAllUsers();
}

// ─────────────────────────────────────────
//  User Storage
// ─────────────────────────────────────────

vector<sUser> loadAllUsers(const string& fileName)
{
    vector<sUser> users;
    fstream       file(fileName, ios::in);

    if (!file.is_open()) return users;

    string line;
    while (getline(file, line))
    {
        if (!line.empty())
            users.push_back(parseUserLine(line));
    }
    file.close();
    return users;
}

void saveAllUsers(const vector<sUser>& users, const string& fileName)
{
    fstream file(fileName, ios::out);
    if (!file.is_open()) return;

    for (const auto& user : users)
        file << serializeUser(user) << "\n";

    file.close();
}

sUser getUserById(const string& userId)
{
    for (const auto& user : loadAllUsers())
        if (user.userId == userId)
            return user;
    return sUser{};
}

// ─────────────────────────────────────────
//  Transaction Storage
// ─────────────────────────────────────────

vector<sTransaction> loadAllTransactions(const string& fileName)
{
    vector<sTransaction> transactions;
    fstream              file(fileName, ios::in);

    if (!file.is_open()) return transactions;

    string line;
    while (getline(file, line))
    {
        if (!line.empty())
            transactions.push_back(parseTransactionLine(line));
    }
    file.close();
    return transactions;
}

vector<sTransaction> loadTransactionsByUser(const sUser& user)
{
    vector<sTransaction> all    = loadAllTransactions();
    vector<sTransaction> result;

    for (const auto& tx : all)
        if (tx.accountId == user.userId || tx.targetAccountId == user.userId)
            result.push_back(tx);

    return result;
}

void appendTransaction(const sTransaction& transaction)
{
    fstream file(TRANSACTIONS_FILE, ios::out | ios::app);
    if (!file.is_open()) return;

    file << serializeTransaction(transaction) << "\n";
    file.close();
}

// ─────────────────────────────────────────
//  Balance Updates
// ─────────────────────────────────────────

void applyDeposit(double amount)
{
    vector<sUser> users = loadAllUsers();

    for (auto& user : users)
    {
        if (user.userId == Session::currentUser.userId)
        {
            user.balance               += amount;
            Session::currentUser.balance += amount;
            break;
        }
    }
    saveAllUsers(users);
}

void applyWithdraw(double amount)
{
    vector<sUser> users = loadAllUsers();

    for (auto& user : users)
    {
        if (user.userId == Session::currentUser.userId)
        {
            user.balance               -= amount;
            Session::currentUser.balance -= amount;
            break;
        }
    }
    saveAllUsers(users);
}

void applyTransfer(double amount, const string& recipientId)
{
    vector<sUser> users = loadAllUsers();

    for (auto& user : users)
    {
        if (user.userId == recipientId)
            user.balance += amount;

        if (user.userId == Session::currentUser.userId)
        {
            user.balance               -= amount;
            Session::currentUser.balance -= amount;
        }
    }
    saveAllUsers(users);
}

void applyBorrow(double amount)
{
    vector<sUser> users = loadAllUsers();

    for (auto& user : users)
    {
        if (user.userId == Session::currentUser.userId)
        {
            user.balance               -= amount;
            Session::currentUser.balance -= amount;
            break;
        }
    }
    saveAllUsers(users);
}

// ─────────────────────────────────────────
//  Transaction Recording
// ─────────────────────────────────────────

void recordTransaction(double amount, enTransactionType type, const string& targetAccountId)
{
    sTransaction tx;

    tx.transactionId       = generateUniqueTransactionId();
    tx.performedByUsername = Session::currentUser.username;
    tx.accountId           = Session::currentUser.userId;
    tx.targetAccountId     = targetAccountId;
    tx.type                = type;
    tx.balanceBefore       = Session::currentUser.balance;
    tx.amount              = amount;
    tx.dateTime            = getCurrentDateTime();

    switch (type)
    {
    case Deposit:  tx.balanceAfter = tx.balanceBefore + amount; break;
    case Withdraw:
    case Transfer:
    case Borrow:   tx.balanceAfter = tx.balanceBefore - amount; break;
    default:       tx.balanceAfter = tx.balanceBefore;          break;
    }

    appendTransaction(tx);
}
