# SingleSignOn-CRM

SingleSignOn-CRM is a desktop CRM (Customer Relationship Management) application built using Qt and C++. It allows users to log in securely using their Google account via OAuth 2.0, and access a simple, clean CRM dashboard. The application emphasizes a responsive and modern UI experience, built using Qt Widgets, with support for rich visuals and optional animations.

---

## 🖼️ GUI Experience

This project offers a native desktop UI, designed with a focus on clarity, responsiveness, and modularity. Below are the major GUI components:

- ✅ **Login Window with Google OAuth**  
  Opens the system browser to authenticate with Google, and displays clear status messages after login.

- ✅ **Dashboard Panel with Tabs or Sections**  
  Organized into cards/sections like:
  - Profile Overview
  - Customer Summary
  - Task Reminders

- ✅ **Animated Transitions (Optional)**  
  You can enable smooth fade-in transitions or sliding panels using `QPropertyAnimation` or `QGraphicsOpacityEffect`.

- ✅ **Custom Styling**  
  Styled with Qt's built-in stylesheet system (`QSS`) to resemble modern UI themes.

> Screenshots will be updated soon.

![Login GUI](images/login_screen.png)
![Dashboard GUI](images/dashboard.png)

---

## 🔐 Key Features

### 1. Google OAuth 2.0 Login  
Secure sign-in using your Google account with support for desktop application authentication flow.

### 2. Responsive Qt GUI  
A lightweight, native UI designed with Qt Widgets — responsive across platforms (Windows/Linux/macOS).

### 3. Built-in HTTP Server  
A local HTTP server listens on `localhost:8080` to handle Google OAuth redirects securely, without requiring a backend service.

### 4. Modular CRM Dashboard  
Once authenticated, the app transitions to a dashboard window. This dashboard is split into components, and is ready for CRM features like customer info, messages, and task reminders.

### 5. UI Animation Support (Optional)  
With `QPropertyAnimation` and `QGraphicsOpacityEffect`, you can animate widget entry, loading transitions, or panel switches.

---

## ⚙️ Build Instructions

### Requirements
- **Qt 5.15+** or **Qt 6+**
- C++17-compatible compiler
- OpenSSL (needed for HTTPS token requests)

### Using CMake
```bash
mkdir build
cd build
cmake ..
make
./SingleSignOn-CRM
