#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QDateTime>

class ServerControl : public QObject {
    Q_OBJECT

public:
    enum ServerType {
        Apache,
        Nginx,
        MySQL,
        PostgreSQL,
        Custom
    };

    enum ServerStatus {
        Running,
        Stopped,
        Unknown,
        Error
    };

    explicit ServerControl(QObject *parent = nullptr);
    ~ServerControl();

    void startServer(ServerType type, const QString& customCommand = "");
    void stopServer(ServerType type, const QString& customCommand = "");
    void restartServer(ServerType type, const QString& customCommand = "");
    ServerStatus getServerStatus(ServerType type);

    static QString serverTypeToString(ServerType type);
    static QString statusToString(ServerStatus status);

    // WSL specific functions
    bool isWSLAvailable();
    QString getWSLVersion();
    QStringList getAvailableServices();

signals:
    void serverStarted(ServerType type);
    void serverStopped(ServerType type);
    void serverError(ServerType type, const QString& error);
    void outputReceived(const QString& output);
    void statusChanged(ServerType type, ServerStatus status);

private slots:
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleProcessError(QProcess::ProcessError error);
    void handleProcessOutput();
    void checkServerStatuses();

private:
    QProcess* m_process;
    ServerType m_currentType;
    QTimer* m_statusCheckTimer;
    QMap<ServerType, ServerStatus> m_serverStatuses;

    QString getCommand(ServerType type, const QString& action, const QString& customCommand = "");
    void executeCommand(const QString& command);
    void parseStatusOutput(const QString& output, ServerType type);
};

#endif // SERVERCONTROL_H
