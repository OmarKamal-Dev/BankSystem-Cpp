#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>

#define NOMINMAX
#include <windows.h>

#include "Helpers.h"

using namespace std;

// ─────────────────────────────────────────
//  Input Readers
// ─────────────────────────────────────────

int readValidInt()
{
    int num;
    cin >> num;
    while (!cin || num < 0)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\t\t      Invalid Input. Please Try Again.\n";
        cout << "\t\t                                     ";
        cin >> num;
    }
    return num;
}

double readValidDouble()
{
    double num;
    cin >> num;
    while (!cin || num < 0)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\t\t      Invalid Input. Please Try Again.\n";
        cout << "\t\t                                     ";
        cin >> num;
    }
    return num;
}

int readValidIntInRange(int from, int to)
{
    int num;
    cin >> num;
    while (!cin || num < from || num > to)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\t\t      Invalid Input. Please enter a number between "
             << from << " and " << to << ".\n";
        cout << "\t\t                                     ";
        cin >> num;
    }
    return num;
}

string readString()
{
    string str;
    getline(cin >> ws, str);
    return str;
}

string readOptionalString()
{
    string str;
    getline(cin, str);
    return str;
}

// ─────────────────────────────────────────
//  String / Display Helpers
// ─────────────────────────────────────────

static string repeatChar(char c, int count)
{
    return string(count, c);
}

static string padRight(const string& text, int width)
{
    if ((int)text.length() >= width) return text;
    return text + string(width - text.length(), ' ');
}

static string centerText(const string& text, int width)
{
    if ((int)text.length() >= width) return text;
    int totalPadding = width - (int)text.length();
    int left  = totalPadding / 2;
    int right = totalPadding - left;
    return string(left, ' ') + text + string(right, ' ');
}

void displayPanel(const string& title, int totalWidth)
{
    int sideStars  = 5;
    int innerWidth = totalWidth - (sideStars * 2);

    cout << "\t\t " << setfill('*') << setw(totalWidth) << "" << "\n";

    cout << setfill(' ');
    cout << "\t\t *****" << setw(innerWidth) << "" << "*****\n";

    int padding = (int)((innerWidth + (int)title.length()) / 2);
    cout << "\t\t *****" << right << setw(padding) << title
         << left << setw(innerWidth - padding) << "" << "*****\n";

    string deco      = "= = = = = = = = = = = = = = = = = = = =";
    int    decoPad   = (int)((innerWidth + (int)deco.length()) / 2);
    cout << "\t\t *****" << right << setw(decoPad) << deco
         << left << setw(innerWidth - decoPad) << "" << "*****\n";

    cout << "\t\t *****" << setw(innerWidth) << "" << "*****\n";
    cout << "\t\t " << setfill('*') << setw(totalWidth) << "" << "\n";
    cout << setfill(' ');
}

void showSuccessScreen(const string& message)
{
    system("cls");
    cout << "\n\n\n\n\n\n";
    displayPanel(message);
    Sleep(2500);
}

void printUserCard(const sUser& user)
{
    vector<string> lines =
    {
        "ID            : " + user.userId,
        "Username      : " + user.username,
        "Password      : " + user.password,
        "Account Type  : " + string(user.accountType == Admin ? "Admin" : "User"),
        "Permissions   : " + to_string(user.permissions),
        "Balance       : " + formatCurrency(user.balance)
    };

    int maxWidth = 0;
    for (const auto& line : lines)
        if ((int)line.length() > maxWidth)
            maxWidth = (int)line.length();

    string title = user.username + "'s Info";
    if ((int)title.length() > maxWidth)
        maxWidth = (int)title.length();

    int boxWidth = maxWidth + 10;

    cout << "\t\t +" << repeatChar('-', boxWidth) << "+\n";
    cout << "\t\t |" << centerText("[ " + title + " ]", boxWidth) << "|\n";
    cout << "\t\t +" << repeatChar('-', boxWidth) << "+\n";
    cout << "\t\t |" << repeatChar(' ', boxWidth) << "|\n";

    for (const auto& line : lines)
        cout << "\t\t |  " << padRight(line, boxWidth - 2) << "|\n";

    cout << "\t\t +" << repeatChar('-', boxWidth) << "+\n";
}

vector<string> splitString(const string& line, const string& delimiter)
{
    vector<string> tokens;
    string         temp = line;
    size_t         pos  = 0;

    while ((pos = temp.find(delimiter)) != string::npos)
    {
        string token = temp.substr(0, pos);
        if (!token.empty())
            tokens.push_back(token);
        temp.erase(0, pos + delimiter.length());
    }
    if (!temp.empty())
        tokens.push_back(temp);

    return tokens;
}

string formatCurrency(double amount)
{
    double rounded = round(amount * 100.0) / 100.0;

    ostringstream stream;
    stream << fixed << setprecision(2) << rounded;
    string str = stream.str();

    size_t dotPos        = str.find('.');
    string integerPart   = str.substr(0, dotPos);
    string decimalPart   = str.substr(dotPos);
    string formattedInt  = "";
    int    count         = 0;

    for (int i = (int)integerPart.length() - 1; i >= 0; i--)
    {
        if (count == 3) { formattedInt += ','; count = 0; }
        formattedInt += integerPart[i];
        count++;
    }
    reverse(formattedInt.begin(), formattedInt.end());
    return formattedInt + decimalPart;
}

string getTransactionTypeLabel(enTransactionType type)
{
    switch (type)
    {
    case Deposit:  return "Deposit";
    case Withdraw: return "Withdraw";
    case Transfer: return "Transfer";
    case Borrow:   return "Borrow";
    default:       return "Unknown";
    }
}

string getCurrentDateTime()
{
    time_t    now = time(0);
    struct tm ltm;
    localtime_s(&ltm, &now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buffer);
}

bool isUsernameAvailable(const string& username, const vector<sUser>& users)
{
    for (const auto& user : users)
        if (user.username == username)
            return false;   // username is already taken
    return true;            // username is free
}

// ─────────────────────────────────────────
//  Unique ID Generator
// ─────────────────────────────────────────

string generateUniqueId(const string& prefix, int length)
{
    static mt19937 rng(
        static_cast<unsigned int>(time(nullptr)) ^
        static_cast<unsigned int>(clock())
    );

    const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uniform_int_distribution<> dist(0, (int)chars.size() - 1);

    string id = prefix;
    for (int i = 0; i < length; i++)
        id += chars[dist(rng)];

    return id;
}
