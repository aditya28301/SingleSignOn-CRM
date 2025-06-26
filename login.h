#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QObject>
#include <QMessageBox>
#include <QString>
#include "oauthlogin.h"

class login : public QDialog {
    Q_OBJECT

    QLabel* username;
    QLabel* password;
    QLineEdit* enter_username;
    QLineEdit* enter_password;
    QPushButton* submit;
    QPushButton* exit;
    OAuthLogin* oauth;

    static bool isUsernameEntered;
    static bool isPasswordEntered;

    QString m_userEmail;
    QString m_userName;

public:
    explicit login(QWidget* parent = nullptr);
    ~login();

    // Getter methods for user information
    QString getUserEmail() const { return m_userEmail; }
    QString getUserName() const { return m_userName; }

    void showCustomerSearch();

private slots:
    void submit_request();
    void enabledLoginButton();
    void updateLoginButton();
};

#endif // LOGIN_H
