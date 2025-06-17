#include "oauthlogin.h"
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QUrlQuery>

OAuthLogin::OAuthLogin(QObject *parent)
    : QObject(parent),
    ClientId("1091899419008-lc4d3dejgmb4udr61iglgbcca1h9c03g.apps.googleusercontent.com"),
    networkManager(new QNetworkAccessManager(this)),
    replyHandler(nullptr)
{
}

OAuthLogin::~OAuthLogin()
{
    if (replyHandler) {
        replyHandler->deleteLater();
    }
}

QString OAuthLogin::generateCodeVerifier() const
{
    const QString possibleChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    QString verifier;
    for(int i = 0; i < 128; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleChars.length());
        verifier.append(possibleChars.at(index));
    }
    return verifier;
}

QString OAuthLogin::generateCodeChallenge(const QString& codeVerifier) const
{
    QByteArray hash = QCryptographicHash::hash(codeVerifier.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromUtf8(hash.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

void OAuthLogin::startLogin()
{
    if (replyHandler) {
        replyHandler->deleteLater();
    }

    replyHandler = new CustomOAuthHandler(8080, this);

    connect(replyHandler, &CustomOAuthHandler::readyToProceed, this, [this]() {
        emit loginSuccess(m_userEmail, m_userName);
    });

    connect(replyHandler, &QAbstractOAuthReplyHandler::callbackReceived,
            this, &OAuthLogin::handleCallback); // new line

    codeVerifier = generateCodeVerifier();
    QUrl authUrl("https://accounts.google.com/o/oauth2/v2/auth");

    QUrlQuery params;
    params.addQueryItem("client_id", ClientId);
    params.addQueryItem("redirect_uri", "http://localhost:8080");
    params.addQueryItem("response_type", "code");
    params.addQueryItem("scope", "openid email profile");
    params.addQueryItem("code_challenge", generateCodeChallenge(codeVerifier));
    params.addQueryItem("code_challenge_method", "S256");
    authUrl.setQuery(params);

    QDesktopServices::openUrl(authUrl);
}

void OAuthLogin::handleCallback(const QVariantMap& values)
{
    if (values.contains("error")) {
        QString errorMsg = values["error"].toString();
        QString errorDesc = values.value("error_description").toString();
        QString fullError = "Error: " + errorMsg;
        if (!errorDesc.isEmpty()) {
            fullError += " - " + errorDesc;
        }
        emit loginFailed(fullError);
        return;
    }

    if (!values.contains("code")) {
        return;
    }

    authCode = values["code"].toString();
    if (authCode.isEmpty()) {
        emit loginFailed("Empty authorization code received");
        return;
    }

    exchangeCodeForToken();
}

void OAuthLogin::exchangeCodeForToken()
{
    QNetworkRequest request(QUrl("https://oauth2.googleapis.com/token"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("client_id", ClientId);
    params.addQueryItem("code", authCode);
    params.addQueryItem("redirect_uri", "http://localhost:8080");
    params.addQueryItem("grant_type", "authorization_code");
    params.addQueryItem("code_verifier", codeVerifier);

    QNetworkReply *reply = networkManager->post(
        request,
        params.toString(QUrl::FullyEncoded).toUtf8()
        );

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            // emit loginFailed("Network error: " + reply->errorString());
            return;
        }

        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        if (json.isNull() || !json.isObject()) {
            emit loginFailed("Invalid response format from token endpoint");
            return;
        }

        QJsonObject obj = json.object();
        if (obj.contains("error")) {
            QString error = obj["error"].toString();
            QString errorDesc = obj.value("error_description").toString();
            emit loginFailed("Token exchange error: " + error + (errorDesc.isEmpty() ? "" : " - " + errorDesc));
            return;
        }

        accessToken = obj["access_token"].toString();
        if (accessToken.isEmpty()) {
            emit loginFailed("No access token in response");
            return;
        }

        fetchUserInfo();
    });
}

void OAuthLogin::fetchUserInfo()
{
    QNetworkRequest request(QUrl("https://www.googleapis.com/oauth2/v1/userinfo"));
    request.setRawHeader("Authorization", "Bearer " + accessToken.toUtf8());

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit loginFailed("User info error: " + reply->errorString());
            return;
        }

        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        if (json.isNull() || !json.isObject()) {
            emit loginFailed("Invalid user info response format");
            return;
        }

        QJsonObject obj = json.object();
        QString email = obj["email"].toString();
        QString name = obj.value("name").toString("User");

        if (email.isEmpty()) {
            emit loginFailed("No email address in user info");
            return;
        }

        m_userEmail = email;
        m_userName = name;

        if (replyHandler) {
            replyHandler->setUserInfo(email, name);
        }
    });
}
