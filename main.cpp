#include "login.h"
#include "CRM_Dashboard.h"
#include <QApplication>
#include <QFont>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
    // Create QApplication first
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("CRM Dashboard");
    app.setOrganizationName("MyCompany");

    // Enable High DPI support
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef _WIN32
    // Set Windows DPI awareness
    SetProcessDPIAware(); // For all Windows versions
#endif

    // Set Fusion style for better high DPI support
    QApplication::setStyle("Fusion");

    // Set default font
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(9);
    QApplication::setFont(defaultFont);

    // Set menu font
    QFont menuFont = defaultFont;
    menuFont.setBold(true);
    QApplication::setFont(menuFont, "QMenuBar");
    QApplication::setFont(menuFont, "QMenu");

    try {
        login loginWindow;
        CRM_Dashboard* customerWindow = nullptr;

        // Use QObject::connect with proper error handling
        QObject::connect(&loginWindow, &QDialog::accepted, [&loginWindow, &customerWindow, &app]() {
            try {
                qDebug() << "Login accepted, creating CRM Dashboard";

                // Get user info from login window
                QString userEmail = loginWindow.getUserEmail();
                QString userName = loginWindow.getUserName();

                if (userEmail.isEmpty()) {
                    userEmail = "user@example.com";
                }
                if (userName.isEmpty()) {
                    userName = "User";
                }

                qDebug() << "Creating dashboard for user:" << userName << "(" << userEmail << ")";

                if (!customerWindow) {
                    customerWindow = new CRM_Dashboard(nullptr, userEmail, userName);
                    if (customerWindow) {
                        customerWindow->showMaximized(); // Show maximized for better visibility
                        loginWindow.hide(); // Hide login window after successful login
                        qDebug() << "CRM Dashboard created and shown successfully";
                    } else {
                        qDebug() << "Failed to create CRM_Dashboard";
                        QMessageBox::critical(nullptr, "Error", "Failed to create CRM Dashboard");
                        app.quit();
                    }
                }
            } catch (const std::exception& e) {
                qDebug() << "Exception in login accept handler:" << e.what();
                QMessageBox::critical(nullptr, "Error",
                                      QString("Failed to initialize CRM Dashboard:\n%1").arg(e.what()));
                app.quit();
            } catch (...) {
                qDebug() << "Unknown exception in login accept handler";
                QMessageBox::critical(nullptr, "Error",
                                      "Failed to initialize CRM Dashboard: Unknown error");
                app.quit();
            }
        });

        // Also handle rejection to clean up
        QObject::connect(&loginWindow, &QDialog::rejected, [&app]() {
            qDebug() << "Login rejected, exiting application";
            app.quit();
        });

        // Handle application cleanup
        QObject::connect(&app, &QApplication::aboutToQuit, [&customerWindow]() {
            qDebug() << "Application about to quit, cleaning up...";
            if (customerWindow) {
                delete customerWindow;
                customerWindow = nullptr;
            }
        });

        loginWindow.show();
        return app.exec();

    } catch (const std::exception& e) {
        qDebug() << "Fatal exception:" << e.what();
        QMessageBox::critical(nullptr, "Fatal Error",
                              QString("Application failed to start:\n%1").arg(e.what()));
        return 1;
    } catch (...) {
        qDebug() << "Unknown fatal exception";
        QMessageBox::critical(nullptr, "Fatal Error",
                              "Application failed to start: Unknown error");
        return 1;
    }
}
