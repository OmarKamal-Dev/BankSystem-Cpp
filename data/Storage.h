#pragma once
#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include "../core/Types.h"

// ─── File Constants ───────────────────────
extern const std::string USERS_FILE;
extern const std::string TRANSACTIONS_FILE;
extern const std::string FIELD_SEPARATOR;

// ─── Session Loading ─────────────────────
void loadSessionData();

// ─── User Storage ────────────────────────
std::vector<sUser> loadAllUsers(const std::string& fileName = USERS_FILE);
void               saveAllUsers(const std::vector<sUser>& users, const std::string& fileName = USERS_FILE);
sUser              getUserById(const std::string& userId);

// ─── Transaction Storage ─────────────────
std::vector<sTransaction> loadAllTransactions(const std::string& fileName = TRANSACTIONS_FILE);
std::vector<sTransaction> loadTransactionsByUser(const sUser& user);
void                      appendTransaction(const sTransaction& transaction);

// ─── Balance Update Helpers ──────────────
void applyDeposit (double amount);
void applyWithdraw(double amount);
void applyTransfer(double amount, const std::string& recipientId);
void applyBorrow  (double amount);

// ─── Transaction Recording ───────────────
void recordTransaction(double amount, enTransactionType type, const std::string& targetAccountId);

// ─── Unique ID Helpers ───────────────────
std::string generateUniqueUserId();
std::string generateUniqueTransactionId();

#endif
