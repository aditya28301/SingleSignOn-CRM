#ifndef CUSTOMERSEARCH_H
#define CUSTOMERSEARCH_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <memory>
#include <vector>
#include <QTableWidget>
#include <QDialog>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTabWidget>
#include <QListWidget>
#include <QRadioButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QDialogButtonBox>
#include <QPainter>
#include <QDoubleSpinBox>
#include <QtAlgorithms>
#include <QMap>

// Customer data structure
struct Customer {
    QString id;
    QString name;
    QString email;
    QString phone;
    QString company;
    QString address;
    QString city;
    QString country;
    QString status;
    double totalSpent;
    int orderCount;
    QDateTime lastOrderDate;
    QDateTime registrationDate;
    double creditLimit;
    QString segment; // VIP, Regular, New
    QStringList tags;
    double satisfactionScore;
    QString preferredContact;
    QJsonObject customFields;
};

// Advanced search criteria
struct SearchCriteria {
    QString textSearch;
    QString status;
    QString segment;
    QString city;
    QString country;
    double minSpent;
    double maxSpent;
    QDate fromDate;
    QDate toDate;
    int minOrders;
    double minSatisfaction;
    QStringList tags;
    bool exactMatch;
};

// Customer Analytics
class CustomerAnalytics : public QObject {
    Q_OBJECT
public:
    explicit CustomerAnalytics(QObject *parent = nullptr);

    void analyzeCustomers(const std::vector<Customer>& customers);
    QJsonObject getSegmentAnalysis() const;
    QJsonObject getGeographicDistribution() const;
    QJsonObject getRevenueAnalysis() const;
    QJsonObject getSatisfactionMetrics() const;

signals:
    void analysisCompleted();

private:
    QJsonObject m_segmentData;
    QJsonObject m_geoData;
    QJsonObject m_revenueData;
    QJsonObject m_satisfactionData;
};

// Real-time customer data synchronization
class CustomerDataSync : public QObject {
    Q_OBJECT
public:
    explicit CustomerDataSync(QObject *parent = nullptr);

    void startSync();
    void stopSync();
    void syncCustomer(const QString& customerId);

signals:
    void customerUpdated(const Customer& customer);
    void syncError(const QString& error);
    void syncProgress(int percentage);

private slots:
    void performSync();

private:
    QTimer *m_syncTimer;
    QNetworkAccessManager *m_networkManager;
    bool m_syncing;
};

// Customer search widget with advanced features
class CustomerSearch : public QMainWindow {
    Q_OBJECT
public:
    explicit CustomerSearch(QWidget *parent = nullptr);
    ~CustomerSearch();

    void loadCustomers();
    void setSearchCriteria(const SearchCriteria& criteria);

public slots:
    // Search operations
    void performSearch();
    void performAdvancedSearch();
    void clearSearch();
    void exportResults();
    void importCustomers();

    // Customer operations
    void viewCustomerDetails();
    void editCustomer();
    void deleteCustomer();
    void mergeCustomers();
    void createCustomer();

    // Analytics
    void showAnalytics();
    void generateReport();

    // Real-time updates
    void handleCustomerUpdate(const Customer& customer);
    void toggleRealTimeSync();

    // Communication
    void sendEmail();
    void sendSMS();
    void scheduleCall();
    void viewCommunicationHistory();

signals:
    void customerSelected(const QString& customerId);
    void searchCompleted(int resultCount);

private:
    void setupUI();
    void setupSearchPanel();
    void setupResultsTable();
    void setupAnalyticsPanel();
    void setupToolBar();
    void connectSignals();
    void applySearchCriteria();
    void updateCustomerInModel(const Customer& customer);
    void highlightSearchResults();

    // UI Components
    QTableView *m_resultsTable;
    QStandardItemModel *m_resultsModel;
    QSortFilterProxyModel *m_proxyModel;

    // Search controls
    QLineEdit *m_searchEdit;
    QComboBox *m_statusFilter;
    QComboBox *m_segmentFilter;
    QComboBox *m_countryFilter;
    QSpinBox *m_minSpentSpin;
    QSpinBox *m_maxSpentSpin;
    QDateEdit *m_fromDateEdit;
    QDateEdit *m_toDateEdit;
    QCheckBox *m_exactMatchCheck;
    QPushButton *m_searchButton;
    QPushButton *m_advancedSearchButton;

    // Analytics display
    QLabel *m_totalCustomersLabel;
    QLabel *m_totalRevenueLabel;
    QLabel *m_avgOrderValueLabel;
    QLabel *m_retentionRateLabel;
    QGroupBox *m_segmentChart;
    QGroupBox *m_geoChart;

    // Data management
    std::vector<Customer> m_customers;
    std::unique_ptr<CustomerAnalytics> m_analytics;
    std::unique_ptr<CustomerDataSync> m_dataSync;
    SearchCriteria m_currentCriteria;

    // Real-time sync
    bool m_realTimeSyncEnabled;
    QTimer *m_autoSaveTimer;
};

// Customer details dialog
class CustomerDetailsDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomerDetailsDialog(const Customer& customer, QWidget *parent = nullptr);

private:
    void setupUI();
    void loadCustomerData();
    void loadOrderHistory();
    void loadCommunicationHistory();
    void calculateMetrics();

    Customer m_customer;
    QTabWidget *m_tabWidget;

    // Info tab
    QLabel *m_nameLabel;
    QLabel *m_emailLabel;
    QLabel *m_phoneLabel;
    QLabel *m_companyLabel;
    QLabel *m_addressLabel;
    QLabel *m_segmentLabel;
    QLabel *m_statusLabel;

    // Orders tab
    QTableView *m_ordersTable;
    QStandardItemModel *m_ordersModel;

    // Communication tab
    QListWidget *m_communicationList;

    // Analytics tab
    QLabel *m_lifetimeValueLabel;
    QLabel *m_avgOrderLabel;
    QLabel *m_frequencyLabel;
    QLabel *m_lastOrderLabel;
};

// Advanced search dialog
class AdvancedSearchDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdvancedSearchDialog(QWidget *parent = nullptr);

    SearchCriteria getSearchCriteria() const;
    void setSearchCriteria(const SearchCriteria& criteria);

private:
    void setupUI();

    QLineEdit *m_keywordEdit;
    QComboBox *m_fieldCombo;
    QComboBox *m_operatorCombo;
    QListWidget *m_conditionsList;
    QPushButton *m_addConditionButton;
    QPushButton *m_removeConditionButton;

    // Filters
    QGroupBox *m_demographicsGroup;
    QGroupBox *m_behavioralGroup;
    QGroupBox *m_transactionalGroup;

    // Demographics
    QComboBox *m_countryCombo;
    QComboBox *m_cityCombo;
    QComboBox *m_segmentCombo;

    // Behavioral
    QSpinBox *m_minOrdersSpin;
    QSpinBox *m_maxOrdersSpin;
    QDoubleSpinBox *m_minSatisfactionSpin;
    QDateEdit *m_lastActivityFromEdit;
    QDateEdit *m_lastActivityToEdit;

    // Transactional
    QDoubleSpinBox *m_minSpentSpin;
    QDoubleSpinBox *m_maxSpentSpin;
    QDoubleSpinBox *m_minAvgOrderSpin;
    QDoubleSpinBox *m_maxAvgOrderSpin;
};

// Customer merge dialog
class CustomerMergeDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomerMergeDialog(const QStringList& customerIds, QWidget *parent = nullptr);

    QString getPrimaryCustomerId() const;
    QStringList getMergeFields() const;

private:
    void setupUI();
    void loadCustomerData();
    void previewMerge();

    QStringList m_customerIds;
    QRadioButton *m_primaryButtons[10];
    QTableWidget *m_comparisonTable;
    QCheckBox *m_mergeFieldChecks[20];
    QPushButton *m_previewButton;
    QTextEdit *m_previewText;
};

// Communication history viewer
class CommunicationHistoryDialog : public QDialog {
    Q_OBJECT
public:
    explicit CommunicationHistoryDialog(const QString& customerId, QWidget *parent = nullptr);

private:
    void setupUI();
    void loadHistory();
    void filterByType();
    void filterByDate();

    QString m_customerId;
    QTableView *m_historyTable;
    QStandardItemModel *m_historyModel;
    QComboBox *m_typeFilter;
    QDateEdit *m_fromDateFilter;
    QDateEdit *m_toDateFilter;
    QPushButton *m_exportButton;
};

// Analytics dashboard dialog
class CustomerAnalyticsDashboard : public QDialog {
    Q_OBJECT
public:
    explicit CustomerAnalyticsDashboard(const std::vector<Customer>& customers,
                                        QWidget *parent = nullptr);

private:
    void setupUI();
    void generateCharts();
    void updateMetrics();
    void exportAnalytics();

    std::vector<Customer> m_customers;
    QTabWidget *m_tabWidget;

    // Overview tab
    QGridLayout *m_metricsGrid;
    QLabel *m_totalCustomersLabel;
    QLabel *m_activeCustomersLabel;
    QLabel *m_churnRateLabel;
    QLabel *m_lifetimeValueLabel;

    // Segment analysis tab
    QWidget *m_segmentChart;
    QTableWidget *m_segmentTable;

    // Geographic distribution tab
    QWidget *m_geoMap;
    QTableWidget *m_geoTable;

    // Revenue analysis tab
    QWidget *m_revenueChart;
    QComboBox *m_periodCombo;

    // Satisfaction metrics tab
    QWidget *m_satisfactionChart;
    QProgressBar *m_npsBar;
};

#endif // CUSTOMERSEARCH_H
