# 💼 SingleSignOn-CRM — A Beautiful Qt Desktop App with Google Login

**SingleSignOn-CRM** is a fully native, C++/Qt-based **CRM desktop application** with a stunning GUI and secure Google login via OAuth 2.0. Designed to be clean, animated, and developer-friendly — this app gives you a professional-level frontend with zero web dependencies.

---

## ✨ Why You'll Love It

🔒 **Secure Google Login**  
🎨 **Modern Qt Widgets UI with Styling**  
🎬 **Smooth UI Transitions with Animation**  
📊 **Expandable CRM Dashboard (Contact/Customer View)**  
🧱 **Fully Modular MVC-ish Codebase**  
🖥️ **Native Desktop Experience — Cross-platform**

> ✅ Ideal for devs learning OAuth + Qt GUI  
> 🚀 Ready to be extended into a real CRM or analytics tool

---

## 🎥 GUI Walkthrough

> 🖼️ _Screenshots and screen recordings will be added soon. Suggested folder: `/images` or `/demo`_

| Login Page | Google Auth Redirect | Animated Dashboard |
|------------|----------------------|---------------------|
| ![Login](images/login.png) | ![OAuth](images/oauth.png) | ![Dashboard](images/dashboard.png) |

---

## 🧠 GUI Design Highlights

### 📌 1. Login Page
- **Login with Google** button styled via `QPushButton` + QSS.
- Subtle fade-in animation on load via `QGraphicsOpacityEffect`.
- Feedback messages shown via animated `QLabel`.

### 🌐 2. Embedded OAuth Flow
- Launches default browser for Google login.
- After login, a custom **local HTTP server** captures the token on `http://localhost:8080`.

### 📊 3. Dashboard (Post Login)
- Uses `QStackedWidget` for screen transitions.
- **Animated panel slide-ins** using `QPropertyAnimation`.
- Panels: _User Summary_, _Quick Actions_, _Client Table_ (future).
- Responsive resizing using layouts (`QHBoxLayout`, `QVBoxLayout`).

### 💅 4. Custom Styling (QSS)
```css
QPushButton {
  background-color: #4285F4;
  color: white;
  padding: 8px 16px;
  border-radius: 4px;
}
QPushButton:hover {
  background-color: #3367D6;
}
