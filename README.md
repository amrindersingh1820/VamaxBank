# 🏦 VALMAX Bank - Blockchain Banking System

![C++](https://img.shields.io/badge/Backend-C++-00599C?style=for-the-badge&logo=c%2B%2B)
![C](https://img.shields.io/badge/Core-C-A8B9CC?style=for-the-badge&logo=c)
![HTML5](https://img.shields.io/badge/Frontend-HTML5-E34F26?style=for-the-badge&logo=html5)
![CSS3](https://img.shields.io/badge/Style-CSS3-1572B6?style=for-the-badge&logo=css3)
![JavaScript](https://img.shields.io/badge/Logic-JavaScript-F7DF1E?style=for-the-badge&logo=javascript)

**VALMAX Bank** is a full-stack banking simulation that combines the performance of **C/C++** with a modern, responsive **Web Frontend**. It utilizes a custom-built private blockchain to ensure data integrity and secure transaction history.

---

## ✨ Features

### 🖥️ Modern User Interface
* **Responsive Dashboard:** A dark-mode, card-based UI inspired by modern fintech apps.
* **Visual Data:** Interactive graphs via **Chart.js** and live crypto price widgets.
* **User Experience:** Smooth transition animations, loading states, and sound effects for interactions.
* **Toast Notifications:** Real-time feedback for successful or failed transactions.

### 🔐 Secure Backend (C/C++)
* **Account Management:** Create, update, view, and delete accounts securely.
* **Transaction Engine:** Fast deposit, withdrawal, and peer-to-peer transfer capabilities.
* **Blockchain Integration:** Every transaction is hashed and added to a linked list (blockchain) to prevent tampering.
* **Data Persistence:** User data and blockchain history are saved locally to `.dat` files.

### 🔗 Blockchain Technology
* **Immutable Ledger:** View the complete history of blocks and transactions.
* **Chain Validation:** Built-in cryptographic validation to ensure the blockchain hasn't been altered.

---

## 📸 Screenshots

*(Add your screenshots here. Create a folder named `screenshots` and add images)*

| Login Screen | Dashboard |
|:---:|:---:|
| ![Login](screenshots/login.png) | ![Dashboard](screenshots/dashboard.png) |

---

## 🛠️ Tech Stack

* **Server:** C++ (`httplib` for handling API requests).
* **Core Logic:** C (Structs, Pointers, File Handling, Blockchain Logic).
* **Frontend:** HTML5, CSS3, Vanilla JavaScript.
* **APIs:** CoinGecko API (for live Crypto prices).
* **Libraries:** `Chart.js` (Frontend graphs), `httplib.h` (Backend server).

---

## 🚀 Getting Started

Follow these instructions to run the project on your local machine.

### Prerequisites
* A C++ Compiler (g++ or clang).
* A modern web browser.

### 1. Compile the Backend
Open your terminal in the project root directory and run:

```bash
# Compile the web server and the backend logic
g++ web_server.cpp c_backend/backend.c -o valmax_server -std=c++11 -pthread
```

### 3. Run Server
Start the application:
# Mac/Linux
./valmax_server

# Windows
valmax_server.exe

You should see: Server starting on http://localhost:8080

### 3. Access the Application
Open your web browser and navigate to: http://localhost:8080

### 📂 Project Structure
├── c_backend/           # Core logic written in C
│   ├── backend.c        # Implementation of banking & blockchain functions
│   └── backend.h        # Header file with structs and definitions
├── www/                 # Frontend assets
│   ├── index.html       # Main application structure
│   ├── style.css        # Styling and animations
│   ├── app.js           # Frontend logic, API calls, and Charts
│   └── sounds/          # MP3 files for UI effects
├── web_server.cpp       # C++ server connecting Frontend to C Backend
└── README.md            # Project Documentation

### 🎮 How to Use
Register: Create a new user ID (6-9 digits) and password.

Login: Access your dashboard.

Create Account: You must create a banking account (Name, Balance) inside your user profile.

Transact: Use the Quick Action buttons to Deposit or Withdraw money.

Blockchain: Click "Display Blockchain" to see the raw data blocks created by your transactions.

Validate: Click "Validate Chain" to run a security check on the ledger integrity.

###🛡️ License
This project is for educational purposes only. It is a proof-of-concept for integrating C/C++ legacy systems with modern web technologies.

###Built with ❤️ by 
Amrinder Singh
Devang Bhatt
Garvit Joshi
Rohit Bhist 
###GitHub
amrindersingh1820
Devang-Bhatt
----------
--------

