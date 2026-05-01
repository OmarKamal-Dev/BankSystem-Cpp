#pragma once
#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <vector>
#include "Types.h"

bool        isUsernameValid  (const std::string& username, const std::vector<sUser>& users);
bool        isPasswordCorrect(int userIndex, const std::string& password, const std::vector<sUser>& users);
int         findUserIndex    (const std::string& username, const std::vector<sUser>& users);
sUser       findUserByName   (const std::string& username);

std::string promptUsername();
std::string promptPassword (const std::string& username);

#endif 
