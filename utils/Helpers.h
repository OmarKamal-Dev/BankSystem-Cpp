#pragma once
#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>
#include "../core/Types.h"

// ─── Input Readers ───────────────────────
std::string readString();
std::string readOptionalString();
int         readValidInt();
int         readValidIntInRange(int from, int to);
double      readValidDouble();

// ─── String / Display Helpers ────────────
std::string              formatCurrency(double amount);
std::string              getTransactionTypeLabel(enTransactionType type);
std::string              getCurrentDateTime();
std::vector<std::string> splitString(const std::string& line, const std::string& delimiter);

void displayPanel(const std::string& title, int totalWidth = 85);
void showSuccessScreen(const std::string& message);
void printUserCard(const sUser& user);

// ─── Validation ──────────────────────────
bool isUsernameAvailable(const std::string& username, const std::vector<sUser>& users);

// ─── ID Generation ───────────────────────
std::string generateUniqueId(const std::string& prefix, int length = 8);

#endif 
