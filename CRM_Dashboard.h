#ifndef CRM_DASHBOARD_H
#define CRM_DASHBOARD_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

class CRM_Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit CRM_Dashboard(QWidget *parent = nullptr, const QString &userEmail = "", const QString &userName = "");
    ~CRM_Dashboard();

private slots:
    void addCustomer();
    void searchCustomer();
    void updateCustomer();
    void deleteCustomer();
    void refreshCustomerList();
    void showAnalytics();
    void exportData();
    void printData();
    void about();

private:
    void setupUI();
    void createMenu();
    void createToolBar();
    void createStatusBar();
    void populateSampleData();

    // Main Widgets
    QTabWidget *mainTabs;
    QTableWidget *customerTable;

    // Customer Management Widgets
    QLineEdit *searchField;
    QComboBox *searchFilter;
    QPushButton *searchButton;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *refreshButton;

    // User Info
    QString currentUserEmail;
    QString currentUserName;

    // Menu Actions
    QAction *actAddCustomer;
    QAction *actSearchCustomer;
    QAction *actRefresh;
    QAction *actExport;
    QAction *actPrint;
    QAction *actAnalytics;
    QAction *actAbout;
    QAction *actExit;
};

#endif // CRM_DASHBOARD_H
