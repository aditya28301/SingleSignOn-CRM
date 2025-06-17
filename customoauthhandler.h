#ifndef CUSTOMOAUTHHANDLER_H
#define CUSTOMOAUTHHANDLER_H

#include <QAbstractOAuthReplyHandler>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrlQuery>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QNetworkReply>

class CustomOAuthHandler : public QAbstractOAuthReplyHandler
{
    Q_OBJECT

public:
    explicit CustomOAuthHandler(quint16 port, QObject *parent = nullptr);
    ~CustomOAuthHandler() override;

    void setUserInfo(const QString& email, const QString& name);
    QString callback() const override;
    void networkReplyFinished(QNetworkReply *reply) override;

signals:
     void readyToProceed();
private slots:
    void handleNewConnection();

private:
    quint16 m_port;
    QTcpServer *m_server;
    QString m_userEmail;
    QString m_userName;

    void handleRequest(QTcpSocket *socket);
    void sendHttpResponse(QTcpSocket *socket, const QByteArray &response);
    void sendSuccessPage(QTcpSocket *socket);
    QString getSuccessHtml() const;
};

#endif // CUSTOMOAUTHHANDLER_H
