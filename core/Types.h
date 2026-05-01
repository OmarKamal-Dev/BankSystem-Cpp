#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

// ─────────────────────────────────────────
//  Enumerations
// ─────────────────────────────────────────

enum enTransactionType
{
    Deposit  = 1,
    Withdraw = 2,
    Transfer = 3,
    Borrow   = 4
};

enum enAccountType
{
    Admin = 1,
    User  = 2
};

// Bit-flag permissions (use bitwise OR to combine)
enum enPermission
{
    PermDeposit        = 1,
    PermWithdraw       = 2,
    PermTransfer       = 4,
    PermBorrow         = 8,
    PermUpdateProfile  = 16,
    PermAdminDashboard = 32
};

// ─────────────────────────────────────────
//  Data Structures
// ─────────────────────────────────────────

struct sUser
{
    std::string   userId;
    std::string   username;
    std::string   password;
    enAccountType accountType;
    short         permissions;
    double        balance;
};

struct sTransaction
{
    std::string        transactionId;
    std::string        performedByUsername;   
    std::string        accountId;             
    std::string        targetAccountId;       // destination account ("0000" = bank)
    enTransactionType  type;
    double             balanceBefore;
    double             amount;
    double             balanceAfter;
    std::string        dateTime;
};

// ─────────────────────────────────────────
//  Session (global state)
// ─────────────────────────────────────────

namespace Session
{
    extern sUser              currentUser;
    extern std::vector<sUser> accounts;
}

#endif 
