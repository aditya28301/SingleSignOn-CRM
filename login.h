#ifndef LOGIN_H
#define LOGIN_H

#include "oauthlogin.h"
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QApplication>

class search_customer;
class login :public QDialog
{
    Q_OBJECT

public:
    login(QWidget* parent = 0);
    ~login();
    void showCustomerSearch();

signals:
    void proceedToMainApp();

private slots:
    void enabledLoginButton();
    void submit_request();

private:
    OAuthLogin* oauth;
    QLabel* username;
    QLabel* password;
    QLineEdit* enter_username;
    QLineEdit* enter_password;
    QPushButton* submit;
    QPushButton* exit;
    static bool isUsernameEntered;
    static bool isPasswordEntered;
    search_customer* customerWindow = nullptr;
    QString m_userEmail;
    QString m_userName;
};
#endif // LOGIN_H
