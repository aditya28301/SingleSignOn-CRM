#include "customoauthhandler.h"
#include <QDebug>
#include <QHostAddress>

CustomOAuthHandler::CustomOAuthHandler(quint16 port, QObject *parent)
    : QAbstractOAuthReplyHandler(parent), m_port(port), m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &CustomOAuthHandler::handleNewConnection);
    if (!m_server->listen(QHostAddress::Any, m_port)) {
        qWarning() << "Failed to start OAuth callback server on port" << m_port;
        qWarning() << "Trying random port...";
        if (!m_server->listen(QHostAddress::LocalHost)) {
            qCritical() << "Could not start server on any port";
        } else {
            m_port = m_server->serverPort();
            qDebug() << "Server started on port" << m_port;
        }
    }
}

CustomOAuthHandler::~CustomOAuthHandler()
{
    m_server->close();
}

void CustomOAuthHandler::setUserInfo(const QString& email, const QString& name)
{
    m_userEmail = email;
    m_userName = name;
}

QString CustomOAuthHandler::callback() const
{
    return QString("http://localhost:%1/").arg(m_port);
}

void CustomOAuthHandler::networkReplyFinished(QNetworkReply *reply)
{
    Q_UNUSED(reply);
}

void CustomOAuthHandler::handleNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            handleRequest(socket);
        });
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}

void CustomOAuthHandler::handleRequest(QTcpSocket *socket)
{
    const QByteArray request = socket->readAll();
    const QByteArray requestLine = request.left(request.indexOf('\r')); // new procee learn

    if (request.startsWith("GET /close")) {
        sendHttpResponse(socket, "HTTP/1.1 200 OK\r\n\r\n");
        emit readyToProceed();
        qDebug() << "get close" << m_port;
        return;
    }

    const int queryStart = requestLine.indexOf('?');
    if (queryStart != -1) {
        const QByteArray query = requestLine.mid(queryStart + 1, 
        requestLine.indexOf(' ', queryStart) - queryStart - 1);
        QUrlQuery urlQuery(QString::fromUtf8(query));

        if (urlQuery.hasQueryItem("code") || urlQuery.hasQueryItem("error")) {
            QVariantMap params;
            const auto queryItems = urlQuery.queryItems();
            for (const auto &item : queryItems) {
                params.insert(item.first, item.second);
            }

            if (urlQuery.hasQueryItem("code")) {
                sendSuccessPage(socket);
            } else {
                sendHttpResponse(socket, "HTTP/1.1 200 OK\r\n\r\nError occurred");
            }
             qDebug() << "callback received" << m_port;
            emit callbackReceived(params);
            return;
        }
    }

    sendHttpResponse(socket, "HTTP/1.1 200 OK\r\n\r\nReceived");
}

void CustomOAuthHandler::sendHttpResponse(QTcpSocket *socket, const QByteArray &response)
{
    socket->write(response);
    socket->flush();
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->waitForDisconnected(1000);
    }
}

void CustomOAuthHandler::sendSuccessPage(QTcpSocket *socket)
{
    QString html = getSuccessHtml();
    QByteArray htmlBytes = html.toUtf8();

    const QByteArray response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "Cache-Control: no-cache\r\n"
        "Content-Length: " + QByteArray::number(htmlBytes.size()) + "\r\n"
                                                 "\r\n" + htmlBytes;

    sendHttpResponse(socket, response);
}

QString CustomOAuthHandler::getSuccessHtml() const
{
    QString html = QStringLiteral(
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>Login Successful - CRM Access Granted</title>\n"
        "    <style>\n"
        "        * {\n"
        "            margin: 0;\n"
        "            padding: 0;\n"
        "            box-sizing: border-box;\n"
        "        }\n"
        "        \n"
        "        body {\n"
        "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
        "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
        "            min-height: 100vh;\n"
        "            display: flex;\n"
        "            align-items: center;\n"
        "            justify-content: center;\n"
        "            overflow: hidden;\n"
        "            position: relative;\n"
        "        }\n"
        "        \n"
        "        .background-animation {\n"
        "            position: absolute;\n"
        "            top: 0;\n"
        "            left: 0;\n"
        "            width: 100%;\n"
        "            height: 100%;\n"
        "            background: linear-gradient(45deg, #ff6b6b, #4ecdc4, #45b7d1, #96ceb4, #ffeaa7);\n"
        "            background-size: 400% 400%;\n"
        "            animation: gradientShift 8s ease infinite;\n"
        "            z-index: -2;\n"
        "        }\n"
        "        \n"
        "        .particles {\n"
        "            position: absolute;\n"
        "            top: 0;\n"
        "            left: 0;\n"
        "            width: 100%;\n"
        "            height: 100%;\n"
        "            z-index: -1;\n"
        "        }\n"
        "        \n"
        "        .particle {\n"
        "            position: absolute;\n"
        "            width: 4px;\n"
        "            height: 4px;\n"
        "            background: rgba(255, 255, 255, 0.8);\n"
        "            border-radius: 50%;\n"
        "            animation: float 6s ease-in-out infinite;\n"
        "        }\n"
        "        \n"
        "        .success-container {\n"
        "            background: rgba(255, 255, 255, 0.95);\n"
        "            backdrop-filter: blur(20px);\n"
        "            border-radius: 20px;\n"
        "            padding: 60px 40px;\n"
        "            max-width: 500px;\n"
        "            width: 90%;\n"
        "            text-align: center;\n"
        "            box-shadow: 0 25px 50px rgba(0, 0, 0, 0.2);\n"
        "            border: 1px solid rgba(255, 255, 255, 0.3);\n"
        "            animation: slideInScale 1.2s cubic-bezier(0.175, 0.885, 0.32, 1.275);\n"
        "            position: relative;\n"
        "            overflow: hidden;\n"
        "        }\n"
        "        \n"
        "        .success-icon {\n"
        "            width: 100px;\n"
        "            height: 100px;\n"
        "            margin: 0 auto 30px;\n"
        "            background: linear-gradient(135deg, #4CAF50, #45a049);\n"
        "            border-radius: 50%;\n"
        "            display: flex;\n"
        "            align-items: center;\n"
        "            justify-content: center;\n"
        "            animation: bounceIn 1s ease-out 0.5s both;\n"
        "            box-shadow: 0 10px 30px rgba(76, 175, 80, 0.4);\n"
        "        }\n"
        "        \n"
        "        .checkmark {\n"
        "            width: 50px;\n"
        "            height: 50px;\n"
        "            position: relative;\n"
        "        }\n"
        "        \n"
        "        .checkmark::after {\n"
        "            content: '';\n"
        "            position: absolute;\n"
        "            left: 18px;\n"
        "            top: 10px;\n"
        "            width: 12px;\n"
        "            height: 24px;\n"
        "            border: solid white;\n"
        "            border-width: 0 4px 4px 0;\n"
        "            transform: rotate(45deg);\n"
        "            animation: checkmarkDraw 0.8s ease-out 1.2s both;\n"
        "        }\n"
        "        \n"
        "        .success-title {\n"
        "            color: #2c3e50;\n"
        "            font-size: 32px;\n"
        "            font-weight: 700;\n"
        "            margin-bottom: 15px;\n"
        "            animation: slideUp 0.8s ease-out 0.8s both;\n"
        "            background: linear-gradient(135deg, #667eea, #764ba2);\n"
        "            -webkit-background-clip: text;\n"
        "            -webkit-text-fill-color: transparent;\n"
        "            background-clip: text;\n"
        "        }\n"
        "        \n"
        "        .success-message {\n"
        "            color: #555;\n"
        "            font-size: 18px;\n"
        "            margin-bottom: 30px;\n"
        "            animation: slideUp 0.8s ease-out 1s both;\n"
        "            line-height: 1.6;\n"
        "        }\n"
        "        \n"
        "        .user-info {\n"
        "            background: linear-gradient(135deg, #f8f9fa, #e9ecef);\n"
        "            margin: 30px 0;\n"
        "            padding: 25px;\n"
        "            border-radius: 15px;\n"
        "            border: 1px solid rgba(0, 0, 0, 0.1);\n"
        "            animation: slideUp 0.8s ease-out 1.2s both;\n"
        "        }\n"
        "        \n"
        "        .user-email {\n"
        "            font-size: 18px;\n"
        "            font-weight: 600;\n"
        "            color: #2c3e50;\n"
        "            margin-bottom: 8px;\n"
        "        }\n"
        "        \n"
        "        .user-name {\n"
        "            font-size: 16px;\n"
        "            color: #666;\n"
        "            font-weight: 500;\n"
        "        }\n"
        "        \n"
        "        .continue-button {\n"
        "            background: linear-gradient(135deg, #4CAF50, #45a049);\n"
        "            color: white;\n"
        "            border: none;\n"
        "            padding: 15px 40px;\n"
        "            font-size: 16px;\n"
        "            font-weight: 600;\n"
        "            border-radius: 50px;\n"
        "            cursor: pointer;\n"
        "            transition: all 0.3s ease;\n"
        "            animation: slideUp 0.8s ease-out 1.4s both;\n"
        "            box-shadow: 0 8px 25px rgba(76, 175, 80, 0.3);\n"
        "        }\n"
        "        \n"
        "        .continue-button:hover {\n"
        "            transform: translateY(-2px);\n"
        "            box-shadow: 0 12px 35px rgba(76, 175, 80, 0.4);\n"
        "        }\n"
        "        \n"
        "        @keyframes gradientShift {\n"
        "            0% { background-position: 0% 50%; }\n"
        "            50% { background-position: 100% 50%; }\n"
        "            100% { background-position: 0% 50%; }\n"
        "        }\n"
        "        \n"
        "        @keyframes slideInScale {\n"
        "            0% {\n"
        "                opacity: 0;\n"
        "                transform: translateY(50px) scale(0.8);\n"
        "            }\n"
        "            100% {\n"
        "                opacity: 1;\n"
        "                transform: translateY(0) scale(1);\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        @keyframes bounceIn {\n"
        "            0% {\n"
        "                opacity: 0;\n"
        "                transform: scale(0.3);\n"
        "            }\n"
        "            50% {\n"
        "                opacity: 1;\n"
        "                transform: scale(1.05);\n"
        "            }\n"
        "            70% {\n"
        "                transform: scale(0.9);\n"
        "            }\n"
        "            100% {\n"
        "                opacity: 1;\n"
        "                transform: scale(1);\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        @keyframes checkmarkDraw {\n"
        "            0% {\n"
        "                height: 0;\n"
        "            }\n"
        "            100% {\n"
        "                height: 24px;\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        @keyframes slideUp {\n"
        "            0% {\n"
        "                opacity: 0;\n"
        "                transform: translateY(30px);\n"
        "            }\n"
        "            100% {\n"
        "                opacity: 1;\n"
        "                transform: translateY(0);\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        @keyframes float {\n"
        "            0%, 100% {\n"
        "                transform: translateY(0px) rotate(0deg);\n"
        "                opacity: 1;\n"
        "            }\n"
        "            50% {\n"
        "                transform: translateY(-20px) rotate(180deg);\n"
        "                opacity: 0.8;\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        @media (max-width: 600px) {\n"
        "            .success-container {\n"
        "                padding: 40px 30px;\n"
        "                margin: 20px;\n"
        "            }\n"
        "            \n"
        "            .success-title {\n"
        "                font-size: 28px;\n"
        "            }\n"
        "            \n"
        "            .success-message {\n"
        "                font-size: 16px;\n"
        "            }\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"background-animation\"></div>\n"
        "    <div class=\"particles\" id=\"particles\"></div>\n"
        "    \n"
        "    <div class=\"success-container\">\n"
        "        <div class=\"success-icon\">\n"
        "            <div class=\"checkmark\"></div>\n"
        "        </div>\n"
        "        \n"
        "        <h1 class=\"success-title\">Authentication Successful!</h1>\n"
        "        <p class=\"success-message\">Welcome to CRM System! Your login credentials have been verified successfully.</p>\n"
        "        \n"
        "        <div class=\"user-info\">\n"
        "            <div class=\"user-email\">%1</div>\n"
        "            <div class=\"user-name\">%2</div>\n"
        "        </div>\n"
        "        \n"
        "        <button class=\"continue-button\" onclick=\"proceedToApplication()\">Launch CRM Dashboard</button>\n"
        "    </div>\n"
        "    \n"
        "    <script>\n"
        "        // Create floating particles\n"
        "        function createParticles() {\n"
        "            const particlesContainer = document.getElementById('particles');\n"
        "            const particleCount = 50;\n"
        "            \n"
        "            for (let i = 0; i < particleCount; i++) {\n"
        "                const particle = document.createElement('div');\n"
        "                particle.className = 'particle';\n"
        "                particle.style.left = Math.random() * 100 + '%%';\n"
        "                particle.style.top = Math.random() * 100 + '%%';\n"
        "                particle.style.animationDelay = Math.random() * 6 + 's';\n"
        "                particle.style.animationDuration = (Math.random() * 3 + 3) + 's';\n"
        "                particlesContainer.appendChild(particle);\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        function proceedToApplication() {\n"
        "            // Send close signal to the Qt application\n"
        "            fetch('/close')\n"
        "                .then(() => {\n"
        "                    window.close();\n"
        "                })\n"
        "                .catch((error) => {\n"
        "                    console.log('Close request sent');\n"
        "                    window.close();\n"
        "                });\n"
        "        }\n"
        "        \n"
        "        document.addEventListener('DOMContentLoaded', function() {\n"
        "            createParticles();\n"
        "        });\n"
        "        \n"
        "        // Keyboard interaction\n"
        "        document.addEventListener('keydown', function(e) {\n"
        "            if (e.key === 'Enter' || e.key === ' ') {\n"
        "                proceedToApplication();\n"
        "            }\n"
        "        });\n"
        "    </script>\n"
        "</body>\n"
        "</html>"
        );

    QString email = m_userEmail.isEmpty() ? "aditya28301@gmail.com" : m_userEmail.toHtmlEscaped();
    QString name = m_userName.isEmpty() ? "Aditya Darekar" : m_userName.toHtmlEscaped();
    return html.arg(email, name);
}
