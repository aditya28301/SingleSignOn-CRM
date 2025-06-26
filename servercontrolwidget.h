#ifndef SERVERCONTROLWIDGET_H
#define SERVERCONTROLWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include "servercontrol.h"

class ServerControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit ServerControlWidget(QWidget *parent = nullptr);
    ~ServerControlWidget();

private slots:
    void onStartClicked();
    void onStopClicked();
    void onRestartClicked();
    void onStatusClicked();
    void onClearLogClicked();
    void onServerOutput(const QString& output);
    void onServerError(ServerControl::ServerType type, const QString& error);
    void onStatusChanged(ServerControl::ServerType type, ServerControl::ServerStatus status);
    void updateWSLInfo();

private:
    void setupUi();
    void createServerStatusWidget();

    ServerControl* m_serverControl;
    QTextEdit* m_logOutput;
    QComboBox* m_serverCombo;
    QPushButton* m_startBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_restartBtn;
    QPushButton* m_statusBtn;
    QPushButton* m_clearBtn;

    // Status indicators
    QLabel* m_apacheStatus;
    QLabel* m_nginxStatus;
    QLabel* m_mysqlStatus;
    QLabel* m_postgresStatus;
    QLabel* m_wslVersionLabel;

    QGroupBox* m_statusGroup;
};

#endif // SERVERCONTROLWIDGET_H
