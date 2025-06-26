#ifndef CRM_DASHBOARD_H
#define CRM_DASHBOARD_H

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QFormLayout>
#include <QDialog>
#include <QSpinBox>
#include <memory>
#include <QSettings>

// Forward declarations
class OrderManager;
class OrderWidget;
class ServerControlWidget;
class CustomerSearch;

// Customer Dialog
class CustomerDialog : public QDialog {
    Q_OBJECT

public:
    explicit CustomerDialog(QWidget *parent = nullptr, bool editMode = false);

    QStringList getCustomerData() const;
    void setCustomerData(const QStringList &data);

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_companyEdit;
    QComboBox *m_statusCombo;
    QSpinBox *m_valueSpin;
    QDateEdit *m_lastContactEdit;
};

// Main CRM Dashboard
class CRM_Dashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit CRM_Dashboard(QWidget *parent = nullptr,
                           const QString &email = QString(),
                           const QString &name = QString());
    ~CRM_Dashboard();

private slots:
    void addCustomer();
    void editCustomer();
    void deleteCustomer();
    void exportCsv();
    void importCsv();
    void showAbout();
    void filterTable();
    void showNotification(const QString &title, const QString &message);
    void toggleFullscreen();
    void updateCustomerTotals();
    void onTabChanged(int index);
    void showCustomerSearch();

private:
    void initUI();
    void buildCentralWidget();
    void createCustomerTab();
    void createOrdersTab();
    void createServerControlTab();
    void createAnalyticsTab();
    void buildToolBar();
    void buildSideDock();
    void buildFilterPanel();
    void createSystemTray();
    void loadSampleData();
    void applyStyleSheet();
    void updateStatus();
    void setupConnections();

    // Core widgets
    QTabWidget *m_mainTabWidget;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxy;
    QTableView *m_table;

    // Filter widgets
    QLineEdit *m_searchEdit;
    QDateEdit *m_dateFrom;
    QDateEdit *m_dateTo;
    QComboBox *m_statusFilter;

    // Statistics labels
    QLabel *m_totalCustomersLabel;
    QLabel *m_totalValueLabel;
    QLabel *m_activeCustomersLabel;
    QLabel *m_totalOrdersLabel;
    QLabel *m_totalRevenueLabel;

    // Feature widgets
    OrderWidget *m_orderWidget;
    ServerControlWidget *m_serverControlWidget;
    CustomerSearch *m_customerSearchWidget;

    // System tray
    QSystemTrayIcon *m_trayIcon;

    // Order manager
    std::shared_ptr<OrderManager> m_orderManager;

    // User info
    QString m_userEmail;
    QString m_userName;
};

#endif // CRM_DASHBOARD_H
