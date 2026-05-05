# 🏦 Bank System — C++ Console Application

A fully featured console-based banking system written in **C++**, supporting both regular users and administrators. Built with a clean modular folder structure and file-based persistent storage.

---

## 📁 Project Structure

```
BankSystem/
├── main.cpp                        ← Entry point (login screen)
│
├── core/
│   ├── Types.h                     ← Structs, enums, Session namespace
│   └── Auth.h / Auth.cpp           ← Login, password validation, user lookup
│
├── data/
│   └── Storage.h / Storage.cpp     ← File I/O, balance updates, transaction recording
│
├── db/
│   ├── users.txt                   ← Persistent user accounts
│   └── transactions.txt            ← Persistent transaction records
│
├── ui/
│   ├── UserDashboard.h/.cpp        ← All regular-user screens and operations
│   └── AdminDashboard.h/.cpp       ← All admin screens and management tools
│
└── utils/
    └── Helpers.h / Helpers.cpp     ← Input readers, formatters, ID generator, UI panels
```

---

## ✨ Features

### 👤 User
| Feature | Description |
|---|---|
| Show Balance | View current account balance |
| Deposit | Add money to your account |
| Withdraw | Withdraw money (checks for sufficient funds) |
| Transfer | Send money to another user |
| Borrow | Borrow up to 10,000 in debt |
| Transaction History | View your personal transaction log |
| Update Profile | Change your username or password securely |

### 🔐 Admin
| Feature | Description |
|---|---|
| Users List | View all registered accounts |
| Add New User | Create a new user with custom permissions |
| Delete User | Remove a user from the system |
| Update User | Modify user data and permissions |
| Find User | Search and display a user's card |
| Full Transaction History | View all transactions with performer, source, and destination |

---

## 🔑 Permission System

Permissions are stored as **bit flags** and can be combined:

| Permission | Value |
|---|---|
| Deposit | 1 |
| Withdraw | 2 |
| Transfer | 4 |
| Borrow | 8 |
| Update Profile | 16 |
| Admin Dashboard | 32 |

Setting permissions to `-1` grants **full access**.

---

## 🗂️ Data Storage

All data is saved in plain text files inside the `db/` folder, using `|` as a field separator:

| File | Contents |
|---|---|
| `db/users.txt` | All user accounts |
| `db/transactions.txt` | All transaction records |

### User record format:
```
UserID|Username|Password|AccountType|Permissions|Balance
```

### Transaction record format:
```
TxID|PerformedBy|SourceAccountID|TargetAccountID|Type|BalBefore|Amount|BalAfter|DateTime
```

---

## 🆔 Unique ID Generation

- **User IDs** are prefixed with `U-` followed by 8 random alphanumeric characters.
- **Transaction IDs** are prefixed with `TX-` followed by 8 random alphanumeric characters.
- Collision detection ensures no two IDs are ever the same.

---

## 🚀 How to Build & Run

### Requirements
- Windows OS (uses `windows.h` for `cls` and `Sleep`)
- C++17 compatible compiler (MSVC, MinGW, or Clang)

### Compile with g++ (MinGW):
```bash
g++ main.cpp core/Auth.cpp data/Storage.cpp ui/UserDashboard.cpp ui/AdminDashboard.cpp utils/Helpers.cpp -o BankSystem -std=c++17 -I. -Icore -Idata -Iui -Iutils
```

### Run:
```bash
./BankSystem
```

### Compile with MSVC (Developer Command Prompt):
```bash
cl main.cpp core\Auth.cpp data\Storage.cpp ui\UserDashboard.cpp ui\AdminDashboard.cpp utils\Helpers.cpp /Fe:BankSystem.exe /std:c++17 /I. /Icore /Idata /Iui /Iutils
```

---

## 🔒 Login Security

- Users have **3 attempts** to enter the correct password.
- After 3 failed attempts, the program displays a freeze warning.
- Passwords are verified against stored records on every login.

---

## 🛠️ Tech Stack

- **Language:** C++17
- **Storage:** File-based (txt files)
- **UI:** Console / ASCII art panels
- **Platform:** Windows

---

## 👨‍💻 Author

**Omar Kamal**  
Built as a hands-on C++ learning project covering file handling, structs, enums, bit-flag permissions, modular design, and console UI.
