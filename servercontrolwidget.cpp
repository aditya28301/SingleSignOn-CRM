#include "servercontrolwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>

ServerControlWidget::ServerControlWidget(QWidget *parent)
    : QWidget(parent), m_serverControl(new ServerControl(this))
{
    setupUi();

    // Connect signals
    connect(m_serverControl, &ServerControl::outputReceived,
            this, &ServerControlWidget::onServerOutput);
    connect(m_serverControl, &ServerControl::serverError,
            this, &ServerControlWidget::onServerError);
    connect(m_serverControl, &ServerControl::statusChanged,
            this, &ServerControlWidget::onStatusChanged);

    // Check WSL availability
    QTimer::singleShot(100, this, &ServerControlWidget::updateWSLInfo);
}

ServerControlWidget::~ServerControlWidget() = default;

void ServerControlWidget::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // WSL Info
    QGroupBox* wslGroup = new QGroupBox(tr("WSL Information"));
    QHBoxLayout* wslLayout = new QHBoxLayout(wslGroup);
    m_wslVersionLabel = new QLabel(tr("Checking WSL..."));
    wslLayout->addWidget(new QLabel(tr("WSL Version:")));
    wslLayout->addWidget(m_wslVersionLabel);
    wslLayout->addStretch();

    mainLayout->addWidget(wslGroup);

    // Server Status
    createServerStatusWidget();
    mainLayout->addWidget(m_statusGroup);

    // Control Panel
    QGroupBox* controlGroup = new QGroupBox(tr("Server Control"));
    QGridLayout* controlLayout = new QGridLayout(controlGroup);

    QLabel* serverLabel = new QLabel(tr("Select Server:"));
    m_serverCombo = new QComboBox();
    m_serverCombo->addItems({"Apache", "Nginx", "MySQL", "PostgreSQL"});

    m_startBtn = new QPushButton(tr("Start"));
    m_stopBtn = new QPushButton(tr("Stop"));
    m_restartBtn = new QPushButton(tr("Restart"));
    m_statusBtn = new QPushButton(tr("Check Status"));

    // Style buttons
    m_startBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    m_stopBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; }");
    m_restartBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; }");
    m_statusBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; }");

    controlLayout->addWidget(serverLabel, 0, 0);
    controlLayout->addWidget(m_serverCombo, 0, 1, 1, 3);
    controlLayout->addWidget(m_startBtn, 1, 0);
    controlLayout->addWidget(m_stopBtn, 1, 1);
    controlLayout->addWidget(m_restartBtn, 1, 2);
    controlLayout->addWidget(m_statusBtn, 1, 3);

    mainLayout->addWidget(controlGroup);

    // Log Output
    QGroupBox* logGroup = new QGroupBox(tr("Server Output"));
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    m_logOutput = new QTextEdit();
    m_logOutput->setReadOnly(true);
    m_logOutput->setFont(QFont("Consolas", 9));
    m_logOutput->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");

    m_clearBtn = new QPushButton(tr("Clear Log"));

    logLayout->addWidget(m_logOutput);
    logLayout->addWidget(m_clearBtn);

    mainLayout->addWidget(logGroup);

    // Connect button signals
    connect(m_startBtn, &QPushButton::clicked, this, &ServerControlWidget::onStartClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &ServerControlWidget::onStopClicked);
    connect(m_restartBtn, &QPushButton::clicked, this, &ServerControlWidget::onRestartClicked);
    connect(m_statusBtn, &QPushButton::clicked, this, &ServerControlWidget::onStatusClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &ServerControlWidget::onClearLogClicked);
}

void ServerControlWidget::createServerStatusWidget() {
    m_statusGroup = new QGroupBox(tr("Server Status"));
    QGridLayout* statusLayout = new QGridLayout(m_statusGroup);

    // Create status labels
    auto createStatusLabel = [](const QString& text) -> QLabel* {
        QLabel* label = new QLabel(text);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { padding: 5px; border-radius: 3px; }");
        return label;
    };

    m_apacheStatus = createStatusLabel("Unknown");
    m_nginxStatus = createStatusLabel("Unknown");
    m_mysqlStatus = createStatusLabel("Unknown");
    m_postgresStatus = createStatusLabel("Unknown");

    statusLayout->addWidget(new QLabel("Apache:"), 0, 0);
    statusLayout->addWidget(m_apacheStatus, 0, 1);
    statusLayout->addWidget(new QLabel("Nginx:"), 0, 2);
    statusLayout->addWidget(m_nginxStatus, 0, 3);
    statusLayout->addWidget(new QLabel("MySQL:"), 1, 0);
    statusLayout->addWidget(m_mysqlStatus, 1, 1);
    statusLayout->addWidget(new QLabel("PostgreSQL:"), 1, 2);
    statusLayout->addWidget(m_postgresStatus, 1, 3);

    statusLayout->setColumnStretch(1, 1);
    statusLayout->setColumnStretch(3, 1);
}

void ServerControlWidget::onStartClicked() {
    auto type = static_cast<ServerControl::ServerType>(m_serverCombo->currentIndex());
    m_serverControl->startServer(type);
}

void ServerControlWidget::onStopClicked() {
    auto type = static_cast<ServerControl::ServerType>(m_serverCombo->currentIndex());
    m_serverControl->stopServer(type);
}

void ServerControlWidget::onRestartClicked() {
    auto type = static_cast<ServerControl::ServerType>(m_serverCombo->currentIndex());
    m_serverControl->restartServer(type);
}

void ServerControlWidget::onStatusClicked() {
    auto type = static_cast<ServerControl::ServerType>(m_serverCombo->currentIndex());
    ServerControl::ServerStatus status = m_serverControl->getServerStatus(type);

    m_logOutput->append(QString("[%1] %2 status: %3")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(m_serverCombo->currentText())
                            .arg(ServerControl::statusToString(status)));
}

void ServerControlWidget::onClearLogClicked() {
    m_logOutput->clear();
}

void ServerControlWidget::onServerOutput(const QString& output) {
    m_logOutput->append(output);
    // Auto-scroll to bottom
    QTextCursor cursor = m_logOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logOutput->setTextCursor(cursor);
}

void ServerControlWidget::onServerError(ServerControl::ServerType type, const QString& error) {
    QString errorMsg = QString("<span style='color: #ff5555;'>[ERROR] %1: %2</span>")
    .arg(ServerControl::serverTypeToString(type))
        .arg(error);
    m_logOutput->append(errorMsg);
}

void ServerControlWidget::onStatusChanged(ServerControl::ServerType type, ServerControl::ServerStatus status) {
    QLabel* statusLabel = nullptr;

    switch (type) {
    case ServerControl::Apache:
        statusLabel = m_apacheStatus;
        break;
    case ServerControl::Nginx:
        statusLabel = m_nginxStatus;
        break;
    case ServerControl::MySQL:
        statusLabel = m_mysqlStatus;
        break;
    case ServerControl::PostgreSQL:
        statusLabel = m_postgresStatus;
        break;
    default:
        return;
    }

    if (!statusLabel) return;

    statusLabel->setText(ServerControl::statusToString(status));

    // Color coding based on status
    switch (status) {
    case ServerControl::Running:
        statusLabel->setStyleSheet("QLabel { background-color: #4CAF50; color: white; padding: 5px; border-radius: 3px; }");
        break;
    case ServerControl::Stopped:
        statusLabel->setStyleSheet("QLabel { background-color: #757575; color: white; padding: 5px; border-radius: 3px; }");
        break;
    case ServerControl::Error:
        statusLabel->setStyleSheet("QLabel { background-color: #f44336; color: white; padding: 5px; border-radius: 3px; }");
        break;
    case ServerControl::Unknown:
        statusLabel->setStyleSheet("QLabel { background-color: #ff9800; color: white; padding: 5px; border-radius: 3px; }");
        break;
    }
}

void ServerControlWidget::updateWSLInfo() {
    if (m_serverControl->isWSLAvailable()) {
        QString version = m_serverControl->getWSLVersion();
        m_wslVersionLabel->setText(version);
        m_wslVersionLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");

        // Get available services
        QStringList services = m_serverControl->getAvailableServices();
        if (!services.isEmpty()) {
            m_logOutput->append(QString("[%1] Available services in WSL:")
                                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
            for (const QString& service : services) {
                m_logOutput->append(QString("  - %1").arg(service));
            }
        }
    } else {
        m_wslVersionLabel->setText(tr("Not Available"));
        m_wslVersionLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");

        QMessageBox::warning(this, tr("WSL Not Available"),
                             tr("Windows Subsystem for Linux (WSL) is not available.\n"
                                "Please install WSL to use server control features.\n\n"
                                "Run 'wsl --install' in PowerShell as Administrator."));
    }
}
