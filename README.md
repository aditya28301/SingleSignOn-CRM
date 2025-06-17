# SingleSignOn-CRM

SingleSignOn-CRM is a desktop CRM (Customer Relationship Management) application built using Qt and C++. The application integrates Google OAuth 2.0 to enable secure login using a Google account, eliminating the need for password management. The goal of this project is to provide a simple and modular desktop CRM system for learning, experimentation, and extension.

This project uses the Qt framework for building cross-platform GUI and networking components. It features a custom-built local HTTP server that listens for the OAuth redirect from Google and processes authentication securely within the app itself.

---

## 🧩 Key Features

### 🔐 Google OAuth 2.0 Integration
The app uses Google's OAuth 2.0 protocol to authenticate users securely. When the login button is clicked, the user's browser opens Google's login page, and after successful login, Google redirects back to a localhost server handled by the app itself.

### 🖥️ Qt-based Graphical Interface
The user interface is created using Qt Widgets. It's designed to be clean, responsive, and extensible. You can easily modify or add new pages like customer dashboards, analytics, or contact forms.

### 📊 CRM Dashboard Module
Once logged in, users are taken to a basic CRM dashboard screen. This screen can be expanded to include client records, data tables, and other CRM-related tools. The dashboard is implemented in a modular way so new widgets or components can be added easily.

### 🧪 Lightweight Local Server for OAuth Redirect
A small embedded HTTP server is included in the application. It listens on `localhost:8080` to catch the OAuth redirect from Google and extract the authorization code. This avoids needing to host any backend services.

### 🔧 Modular and Clean Codebase
The source code is organized by functionality: login logic, OAuth handler, and dashboard. It’s easy to read and modify. The code follows object-oriented principles and uses modern C++ standards.

---

## 🖼️ GUI Screenshots (Coming Soon)

> Once you've added your screenshots, you can place them in a folder named `/images` and update the links below.

- **Login Page**  
  _[Image Placeholder]_  
  ![Login Screen](images/login_screen.png)

- **CRM Dashboard**  
  _[Image Placeholder]_  
  ![Dashboard](images/dashboard.png)

---

## 🛠️ How to Build the Project

### 📋 Prerequisites
To build and run the project, make sure you have the following installed:
- Qt 5 or Qt 6 (with Qt Creator or `qmake` and/or CMake)
- A C++17 compatible compiler (GCC, Clang, or MSVC)
- OpenSSL (required for OAuth HTTPS communication)

### 🧰 Option 1: Build Using CMake (Recommended)
```bash
mkdir build
cd build
cmake ..
make
./SingleSignOn-CRM
