#ifndef OAUTHLOGIN_H
#define OAUTHLOGIN_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>
#include "customoauthhandler.h"
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

class OAuthLogin : public QObject
{
    Q_OBJECT

public:
    explicit OAuthLogin(QObject *parent = nullptr);
    ~OAuthLogin();

    void startLogin();

signals:
    void loginSuccess(const QString& email, const QString& name);
    void loginFailed(const QString& error);

private slots:
    void handleCallback(const QVariantMap& values);

private:
    QString generateCodeVerifier() const;
    QString generateCodeChallenge(const QString& codeVerifier) const;
    void exchangeCodeForToken();
    void fetchUserInfo();

    const QString ClientId;
    QNetworkAccessManager* networkManager;
    CustomOAuthHandler* replyHandler;

    QString codeVerifier;
    QString authCode;
    QString accessToken;
    QString m_userEmail;
    QString m_userName;
};

#endif // OAUTHLOGIN_H
