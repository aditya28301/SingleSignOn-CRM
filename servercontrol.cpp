#include "servercontrol.h"
#include <QDebug>
#include <QRegularExpression>

ServerControl::ServerControl(QObject *parent)
    : QObject(parent), m_process(new QProcess(this))
{
    connect(m_process, &QProcess::finished, this, &ServerControl::handleProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &ServerControl::handleProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &ServerControl::handleProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &ServerControl::handleProcessOutput);

    // Initialize server statuses
    m_serverStatuses[Apache] = Unknown;
    m_serverStatuses[Nginx] = Unknown;
    m_serverStatuses[MySQL] = Unknown;
    m_serverStatuses[PostgreSQL] = Unknown;

    // Setup status check timer
    m_statusCheckTimer = new QTimer(this);
    connect(m_statusCheckTimer, &QTimer::timeout, this, &ServerControl::checkServerStatuses);
    m_statusCheckTimer->start(5000); // Check every 5 seconds

    // Initial status check
    checkServerStatuses();
}

ServerControl::~ServerControl() {
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(5000)) {
            m_process->kill();
        }
    }
}

bool ServerControl::isWSLAvailable() {
    QProcess wslCheck;
    wslCheck.start("wsl", QStringList() << "--version");
    wslCheck.waitForFinished(2000);
    return wslCheck.exitCode() == 0;
}

QString ServerControl::getWSLVersion() {
    QProcess wslCheck;
    wslCheck.start("wsl", QStringList() << "--version");
    wslCheck.waitForFinished(2000);

    if (wslCheck.exitCode() == 0) {
        QString output = wslCheck.readAllStandardOutput();
        QRegularExpression re("WSL version: ([0-9.]+)");
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            return match.captured(1);
        }
    }
    return "Unknown";
}

QStringList ServerControl::getAvailableServices() {
    QStringList services;
    QProcess serviceList;

    serviceList.start("wsl", QStringList() << "service" << "--status-all");
    serviceList.waitForFinished(3000);

    if (serviceList.exitCode() == 0) {
        QString output = serviceList.readAllStandardOutput();
        QStringList lines = output.split('\n');

        for (const QString& line : lines) {
            QRegularExpression re("\\[ [+-] \\]\\s+(.+)");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                services << match.captured(1).trimmed();
            }
        }
    }

    return services;
}

void ServerControl::startServer(ServerType type, const QString& customCommand) {
    m_currentType = type;
    QString cmd = getCommand(type, "start", customCommand);

    emit outputReceived(QString("[%1] Starting %2...")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(serverTypeToString(type)));

    executeCommand(cmd);
}

void ServerControl::stopServer(ServerType type, const QString& customCommand) {
    m_currentType = type;
    QString cmd = getCommand(type, "stop", customCommand);

    emit outputReceived(QString("[%1] Stopping %2...")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(serverTypeToString(type)));

    executeCommand(cmd);
}

void ServerControl::restartServer(ServerType type, const QString& customCommand) {
    m_currentType = type;
    QString cmd = getCommand(type, "restart", customCommand);

    emit outputReceived(QString("[%1] Restarting %2...")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(serverTypeToString(type)));

    executeCommand(cmd);
}

ServerControl::ServerStatus ServerControl::getServerStatus(ServerType type) {
    return m_serverStatuses.value(type, Unknown);
}

QString ServerControl::serverTypeToString(ServerType type) {
    switch (type) {
    case Apache: return "Apache";
    case Nginx: return "Nginx";
    case MySQL: return "MySQL";
    case PostgreSQL: return "PostgreSQL";
    case Custom: return "Custom";
    }
    return "Unknown";
}

QString ServerControl::statusToString(ServerStatus status) {
    switch (status) {
    case Running: return "Running";
    case Stopped: return "Stopped";
    case Unknown: return "Unknown";
    case Error: return "Error";
    }
    return "Unknown";
}

QString ServerControl::getCommand(ServerType type, const QString& action, const QString& customCommand) {
    if (type == Custom && !customCommand.isEmpty()) {
        return QString("wsl %1").arg(customCommand);
    }

    QString service;
    switch (type) {
    case Apache:
        service = "apache2";
        break;
    case Nginx:
        service = "nginx";
        break;
    case MySQL:
        service = "mysql";
        break;
    case PostgreSQL:
        service = "postgresql";
        break;
    default:
        return "";
    }

    // WSL command format
    if (action == "status") {
        return QString("wsl service %1 status").arg(service);
    } else {
        return QString("wsl sudo service %1 %2").arg(service, action);
    }
}

void ServerControl::executeCommand(const QString& command) {
    if (command.isEmpty()) {
        emit serverError(m_currentType, "Invalid command");
        return;
    }

// For Windows, we need to use cmd.exe to run wsl commands
#ifdef Q_OS_WIN
    m_process->start("cmd.exe", QStringList() << "/c" << command);
#else
    m_process->start("bash", QStringList() << "-c" << command);
#endif
}

void ServerControl::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit outputReceived(QString("[%1] Command completed successfully")
                                .arg(timestamp));
        emit serverStarted(m_currentType);

        // Update status after successful command
        QTimer::singleShot(1000, this, &ServerControl::checkServerStatuses);
    } else {
        emit outputReceived(QString("[%1] Command failed with exit code %2")
                                .arg(timestamp)
                                .arg(exitCode));
        emit serverError(m_currentType, QString("Process exited with code %1").arg(exitCode));
    }
}

void ServerControl::handleProcessError(QProcess::ProcessError error) {
    QString errorString;
    switch (error) {
    case QProcess::FailedToStart:
        errorString = "Failed to start process. Make sure WSL is installed.";
        break;
    case QProcess::Crashed:
        errorString = "Process crashed";
        break;
    case QProcess::Timedout:
        errorString = "Process timed out";
        break;
    default:
        errorString = "Unknown error";
    }

    emit outputReceived(QString("[%1] ERROR: %2")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(errorString));

    emit serverError(m_currentType, errorString);
}

void ServerControl::handleProcessOutput() {
    QString output = m_process->readAllStandardOutput();
    QString error = m_process->readAllStandardError();

    if (!output.isEmpty()) {
        // Clean up the output
        output = output.trimmed();
        if (!output.isEmpty()) {
            emit outputReceived(output);
        }
    }

    if (!error.isEmpty()) {
        error = error.trimmed();
        if (!error.isEmpty()) {
            // Check if it's a sudo password prompt
            if (error.contains("sudo") && error.contains("password")) {
                emit outputReceived("[WARNING] Sudo password required. Configure passwordless sudo for service commands.");
            } else {
                emit outputReceived(QString("[ERROR] %1").arg(error));
            }
        }
    }
}

void ServerControl::checkServerStatuses() {
    static const QList<ServerType> serverTypes = {Apache, Nginx, MySQL, PostgreSQL};

    for (ServerType type : serverTypes) {
        QProcess* statusProcess = new QProcess(this);

        connect(statusProcess, &QProcess::finished, [this, statusProcess, type]() {
            QString output = statusProcess->readAllStandardOutput();
            parseStatusOutput(output, type);
            statusProcess->deleteLater();
        });

        QString cmd = getCommand(type, "status");
#ifdef Q_OS_WIN
        statusProcess->start("cmd.exe", QStringList() << "/c" << cmd);
#else
        statusProcess->start("bash", QStringList() << "-c" << cmd);
#endif
    }
}

void ServerControl::parseStatusOutput(const QString& output, ServerType type) {
    ServerStatus newStatus = Unknown;

    if (output.contains("is running", Qt::CaseInsensitive) ||
        output.contains("active (running)", Qt::CaseInsensitive)) {
        newStatus = Running;
    } else if (output.contains("is not running", Qt::CaseInsensitive) ||
               output.contains("inactive", Qt::CaseInsensitive) ||
               output.contains("stopped", Qt::CaseInsensitive)) {
        newStatus = Stopped;
    } else if (output.contains("failed", Qt::CaseInsensitive)) {
        newStatus = Error;
    }

    if (m_serverStatuses[type] != newStatus) {
        m_serverStatuses[type] = newStatus;
        emit statusChanged(type, newStatus);
    }
}

