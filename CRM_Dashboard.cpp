#include "CRM_Dashboard.h"
#include "ordermanager.h"
#include "orderwidget.h"
#include "servercontrolwidget.h"
#include "customer_search.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMenuBar>
#include <QDialogButtonBox>

// CustomerDialog Implementation
CustomerDialog::CustomerDialog(QWidget *parent, bool editMode)
    : QDialog(parent)
{
    setWindowTitle(editMode ? tr("Edit Customer") : tr("Add Customer"));
    setModal(true);
    resize(400, 300);

    QFormLayout *formLayout = new QFormLayout(this);

    m_nameEdit = new QLineEdit();
    m_emailEdit = new QLineEdit();
    m_phoneEdit = new QLineEdit();
    m_companyEdit = new QLineEdit();
    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"Lead", "Prospect", "Customer", "VIP"});
    m_valueSpin = new QSpinBox();
    m_valueSpin->setRange(0, 999999);
    m_valueSpin->setSuffix(" $");
    m_lastContactEdit = new QDateEdit();
    m_lastContactEdit->setDate(QDate::currentDate());
    m_lastContactEdit->setCalendarPopup(true);

    formLayout->addRow(tr("Name:"), m_nameEdit);
    formLayout->addRow(tr("Email:"), m_emailEdit);
    formLayout->addRow(tr("Phone:"), m_phoneEdit);
    formLayout->addRow(tr("Company:"), m_companyEdit);
    formLayout->addRow(tr("Status:"), m_statusCombo);
    formLayout->addRow(tr("Value:"), m_valueSpin);
    formLayout->addRow(tr("Last Contact:"), m_lastContactEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QStringList CustomerDialog::getCustomerData() const
{
    return {
        m_nameEdit->text(),
        m_emailEdit->text(),
        m_phoneEdit->text(),
        m_companyEdit->text(),
        m_statusCombo->currentText(),
        QString::number(m_valueSpin->value()),
        m_lastContactEdit->date().toString("yyyy-MM-dd")
    };
}

void CustomerDialog::setCustomerData(const QStringList &data)
{
    if (data.size() >= 7) {
        m_nameEdit->setText(data[0]);
        m_emailEdit->setText(data[1]);
        m_phoneEdit->setText(data[2]);
        m_companyEdit->setText(data[3]);
        m_statusCombo->setCurrentText(data[4]);
        m_valueSpin->setValue(data[5].toInt());
        m_lastContactEdit->setDate(QDate::fromString(data[6], "yyyy-MM-dd"));
    }
}

// CRM_Dashboard Implementation
CRM_Dashboard::CRM_Dashboard(QWidget *parent, const QString &email, const QString &name)
    : QMainWindow(parent)
    , m_mainTabWidget(nullptr)
    , m_model(nullptr)
    , m_proxy(nullptr)
    , m_table(nullptr)
    , m_searchEdit(nullptr)
    , m_dateFrom(nullptr)
    , m_dateTo(nullptr)
    , m_statusFilter(nullptr)
    , m_orderWidget(nullptr)
    , m_serverControlWidget(nullptr)
    , m_customerSearchWidget(nullptr)
    , m_trayIcon(nullptr)
    , m_userEmail(email)
    , m_userName(name)
    , m_totalCustomersLabel(nullptr)
    , m_totalValueLabel(nullptr)
    , m_activeCustomersLabel(nullptr)
    , m_totalOrdersLabel(nullptr)
    , m_totalRevenueLabel(nullptr)
{
    qDebug() << "CRM_Dashboard constructor started";

    try {
        // Initialize OrderManager FIRST
        m_orderManager = std::make_shared<OrderManager>(this);

        // Set window properties
        setWindowTitle("CRM Dashboard");
        setMinimumSize(800, 600);

        qDebug() << "Calling initUI...";
        initUI();

        qDebug() << "CRM_Dashboard constructor completed successfully";

    } catch (const std::exception& e) {
        qDebug() << "Exception in CRM_Dashboard constructor:" << e.what();
        QMessageBox::critical(this, "Initialization Error",
                              QString("Failed to initialize CRM Dashboard: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Unknown exception in CRM_Dashboard constructor";
        QMessageBox::critical(this, "Initialization Error",
                              "Failed to initialize CRM Dashboard: Unknown error");
    }
}

CRM_Dashboard::~CRM_Dashboard()
{
    QSettings settings("MyCompany", "CRM");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void CRM_Dashboard::initUI()
{
    qDebug() << "initUI() started";

    try {
        // Create central widget first
        qDebug() << "Creating central widget...";
        buildCentralWidget();

        qDebug() << "Building toolbar...";
        buildToolBar();

        qDebug() << "Building side dock...";
        buildSideDock();

        qDebug() << "Creating system tray...";
        createSystemTray();

        qDebug() << "Loading sample data...";
        loadSampleData();

        qDebug() << "Applying stylesheet...";
        applyStyleSheet();

        qDebug() << "Setting up connections...";
        setupConnections();

        qDebug() << "Updating status...";
        updateStatus();

        qDebug() << "initUI() completed successfully";

    } catch (const std::exception& e) {
        qDebug() << "Exception in initUI():" << e.what();
        throw;
    } catch (...) {
        qDebug() << "Unknown exception in initUI()";
        throw;
    }
}

void CRM_Dashboard::buildCentralWidget()
{
    qDebug() << "buildCentralWidget() started";

    try {
        // Create main tab widget safely
        m_mainTabWidget = new QTabWidget(this);
        if (!m_mainTabWidget) {
            throw std::runtime_error("Failed to create main tab widget");
        }

        setCentralWidget(m_mainTabWidget);

        // Create tabs one by one with error checking
        qDebug() << "Creating customer tab...";
        createCustomerTab();

        qDebug() << "Creating orders tab...";
        createOrdersTab();

        qDebug() << "Creating server control tab...";
        createServerControlTab();

        qDebug() << "Creating analytics tab...";
        createAnalyticsTab();

        // Connect tab change signal
        connect(m_mainTabWidget, &QTabWidget::currentChanged,
                this, &CRM_Dashboard::onTabChanged);

        qDebug() << "buildCentralWidget() completed successfully";

    } catch (const std::exception& e) {
        qDebug() << "Exception in buildCentralWidget():" << e.what();
        throw;
    } catch (...) {
        qDebug() << "Unknown exception in buildCentralWidget()";
        throw;
    }
}

void CRM_Dashboard::createCustomerTab()
{
    qDebug() << "createCustomerTab() started";

    try {
        // Create model safely
        m_model = new QStandardItemModel(this);
        if (!m_model) {
            throw std::runtime_error("Failed to create standard item model");
        }

        // Set headers
        m_model->setHorizontalHeaderLabels({
            "Name", "Email", "Phone", "Company", "Status", "Value", "Last Contact"
        });

        // Create proxy model
        m_proxy = new QSortFilterProxyModel(this);
        if (!m_proxy) {
            throw std::runtime_error("Failed to create sort filter proxy model");
        }
        m_proxy->setSourceModel(m_model);
        m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

        // Create table view
        m_table = new QTableView();
        if (!m_table) {
            throw std::runtime_error("Failed to create table view");
        }
        m_table->setModel(m_proxy);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setAlternatingRowColors(true);
        m_table->setSortingEnabled(true);
        m_table->horizontalHeader()->setStretchLastSection(true);

        // Add to tab widget
        if (m_mainTabWidget) {
            m_mainTabWidget->addTab(m_table, "Customers");
        }

        qDebug() << "createCustomerTab() completed successfully";

    } catch (const std::exception& e) {
        qDebug() << "Exception in createCustomerTab():" << e.what();
        throw;
    } catch (...) {
        qDebug() << "Unknown exception in createCustomerTab()";
        throw;
    }
}

void CRM_Dashboard::createOrdersTab()
{
    qDebug() << "createOrdersTab() started";

    try {
        if (m_orderManager) {
            m_orderWidget = new OrderWidget(m_orderManager.get());
            m_mainTabWidget->addTab(m_orderWidget, tr("Orders"));
            qDebug() << "Orders tab created successfully";
        } else {
            // Create a placeholder widget if OrderManager is not available
            QWidget *placeholderWidget = new QWidget();
            QVBoxLayout *layout = new QVBoxLayout(placeholderWidget);
            QLabel *label = new QLabel(tr("Order management not available"));
            label->setAlignment(Qt::AlignCenter);
            layout->addWidget(label);
            m_mainTabWidget->addTab(placeholderWidget, tr("Orders"));
            qDebug() << "Orders tab created with placeholder";
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception in createOrdersTab():" << e.what();
        // Create error placeholder
        QWidget *errorWidget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(errorWidget);
        QLabel *label = new QLabel(tr("Error loading orders: %1").arg(e.what()));
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        m_mainTabWidget->addTab(errorWidget, tr("Orders"));
    }
}

void CRM_Dashboard::createServerControlTab()
{
    try {
        m_serverControlWidget = new ServerControlWidget();
        m_mainTabWidget->addTab(m_serverControlWidget, tr("Server Control"));
    } catch (const std::exception& e) {
        qDebug() << "Failed to create ServerControlWidget:" << e.what();
        // Create placeholder widget
        QWidget *placeholderWidget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(placeholderWidget);
        QLabel *label = new QLabel(tr("Server control not available"));
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        m_mainTabWidget->addTab(placeholderWidget, tr("Server Control"));
    }
}

void CRM_Dashboard::createAnalyticsTab()
{
    qDebug() << "createAnalyticsTab() started - using temporary placeholder";

    // Create simple placeholder for now
    QWidget *placeholderWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(placeholderWidget);

    QLabel *titleLabel = new QLabel(tr("Analytics & Search"), placeholderWidget);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; text-align: center; margin: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *messageLabel = new QLabel(tr("Analytics features coming soon..."), placeholderWidget);
    messageLabel->setStyleSheet("color: #666; text-align: center; margin: 10px;");
    messageLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();

    m_customerSearchWidget = nullptr; // Don't point to this placeholder

    m_mainTabWidget->addTab(placeholderWidget, tr("Analytics & Search"));

    qDebug() << "createAnalyticsTab() completed with placeholder";
}

void CRM_Dashboard::buildFilterPanel()
{
    m_dateFrom = new QDateEdit();
    m_dateFrom->setDate(QDate::currentDate().addDays(-30));
    m_dateFrom->setCalendarPopup(true);

    m_dateTo = new QDateEdit();
    m_dateTo->setDate(QDate::currentDate());
    m_dateTo->setCalendarPopup(true);

    m_statusFilter = new QComboBox();
    m_statusFilter->addItems({"All", "Lead", "Prospect", "Customer", "VIP"});
}

void CRM_Dashboard::buildToolBar()
{
    auto *tb = addToolBar(tr("Actions"));
    tb->setObjectName("ActionsToolBar");
    tb->setIconSize(QSize(32, 32)); // Larger icons for better visibility
    tb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tb->setMovable(false); // Prevent accidental moving

    // Use more appropriate icons and ensure they're visible
    QAction *actAdd = tb->addAction(
        style()->standardIcon(QStyle::SP_FileIcon), // Better icon choice
        tr("Add Customer"));
    actAdd->setToolTip(tr("Add a new customer to the database"));

    QAction *actEdit = tb->addAction(
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        tr("Edit"));
    actEdit->setToolTip(tr("Edit selected customer"));

    QAction *actDel = tb->addAction(
        style()->standardIcon(QStyle::SP_TrashIcon),
        tr("Delete"));
    actDel->setToolTip(tr("Delete selected customer"));

    tb->addSeparator();

    QAction *actSearch = tb->addAction(
        style()->standardIcon(QStyle::SP_FileDialogContentsView),
        tr("Search"));
    actSearch->setToolTip(tr("Advanced customer search"));

    QAction *actImport = tb->addAction(
        style()->standardIcon(QStyle::SP_DirOpenIcon), // Better icon
        tr("Import"));
    actImport->setToolTip(tr("Import customers from CSV file"));

    QAction *actExport = tb->addAction(
        style()->standardIcon(QStyle::SP_DialogSaveButton),
        tr("Export"));
    actExport->setToolTip(tr("Export customers to CSV file"));

    tb->addSeparator();

    QAction *actRefresh = tb->addAction(
        style()->standardIcon(QStyle::SP_BrowserReload),
        tr("Refresh"));
    actRefresh->setToolTip(tr("Refresh customer data"));

    // Search widget in toolbar with better styling
    QWidget *searchWidget = new QWidget();
    searchWidget->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(10, 5, 10, 5);
    searchLayout->setSpacing(8);

    QLabel *searchLabel = new QLabel(tr("Search:"));
    searchLabel->setStyleSheet("QLabel { font-weight: 600; color: #495057; }");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Search customers..."));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(280);
    m_searchEdit->setMaximumWidth(350);

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addStretch(); // Push to the right

    tb->addWidget(searchWidget);

    // Connect actions
    connect(actAdd, &QAction::triggered, this, &CRM_Dashboard::addCustomer);
    connect(actEdit, &QAction::triggered, this, &CRM_Dashboard::editCustomer);
    connect(actDel, &QAction::triggered, this, &CRM_Dashboard::deleteCustomer);
    connect(actSearch, &QAction::triggered, this, &CRM_Dashboard::showCustomerSearch);
    connect(actExport, &QAction::triggered, this, &CRM_Dashboard::exportCsv);
    connect(actImport, &QAction::triggered, this, &CRM_Dashboard::importCsv);
    connect(actRefresh, &QAction::triggered, this, [this]() {
        updateCustomerTotals();
        showNotification(tr("Refresh"), tr("Data refreshed"));
    });

    if (m_searchEdit && m_proxy) {
        connect(m_searchEdit, &QLineEdit::textChanged,
                m_proxy, &QSortFilterProxyModel::setFilterFixedString);
    }
}

void CRM_Dashboard::buildSideDock()
{
    auto *dock = new QDockWidget(tr("Dashboard Summary"), this);
    dock->setObjectName("DashboardSummaryDock");

    auto *widget = new QWidget();
    auto *layout = new QVBoxLayout(widget);

    // Customer Statistics
    auto *customerStatsGroup = new QGroupBox(tr("Customer Statistics"));
    auto *customerStatsLayout = new QGridLayout(customerStatsGroup);

    m_totalCustomersLabel = new QLabel("0");
    m_totalValueLabel = new QLabel("$0");
    m_activeCustomersLabel = new QLabel("0");

    QFont statFont;
    statFont.setPointSize(14);
    statFont.setBold(true);

    m_totalCustomersLabel->setFont(statFont);
    m_totalValueLabel->setFont(statFont);
    m_activeCustomersLabel->setFont(statFont);

    customerStatsLayout->addWidget(new QLabel(tr("Total Customers:")), 0, 0);
    customerStatsLayout->addWidget(m_totalCustomersLabel, 0, 1);
    customerStatsLayout->addWidget(new QLabel(tr("Total Value:")), 1, 0);
    customerStatsLayout->addWidget(m_totalValueLabel, 1, 1);
    customerStatsLayout->addWidget(new QLabel(tr("Active:")), 2, 0);
    customerStatsLayout->addWidget(m_activeCustomersLabel, 2, 1);

    layout->addWidget(customerStatsGroup);

    // Order Statistics
    auto *orderStatsGroup = new QGroupBox(tr("Order Statistics"));
    auto *orderStatsLayout = new QGridLayout(orderStatsGroup);

    m_totalOrdersLabel = new QLabel("0");
    m_totalRevenueLabel = new QLabel("$0");

    m_totalOrdersLabel->setFont(statFont);
    m_totalRevenueLabel->setFont(statFont);

    orderStatsLayout->addWidget(new QLabel(tr("Total Orders:")), 0, 0);
    orderStatsLayout->addWidget(m_totalOrdersLabel, 0, 1);
    orderStatsLayout->addWidget(new QLabel(tr("Total Revenue:")), 1, 0);
    orderStatsLayout->addWidget(m_totalRevenueLabel, 1, 1);

    layout->addWidget(orderStatsGroup);

    // Recent Activity
    auto *activityGroup = new QGroupBox(tr("Recent Activity"));
    auto *activityLayout = new QVBoxLayout(activityGroup);

    auto *activityList = new QListWidget();
    activityList->addItems({
        tr("New order from Aditya Darekar - $899.99"),
        tr("Customer Somesh Atole status changed to VIP"),
        tr("Server Apache started successfully"),
        tr("5 new customers imported"),
        tr("Monthly report generated")
    });
    activityLayout->addWidget(activityList);

    layout->addWidget(activityGroup);
    layout->addStretch();

    widget->setLayout(layout);
    dock->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // FIX: Add null check before connecting signals
    if (m_orderManager) {
        // Update order statistics
        connect(m_orderManager.get(), &OrderManager::orderAdded, this, [this]() {
            if (m_totalOrdersLabel && m_totalRevenueLabel && m_orderManager) {
                m_totalOrdersLabel->setText(QString::number(m_orderManager->getTotalOrderCount()));
                m_totalRevenueLabel->setText(QString("$%1").arg(m_orderManager->getTotalRevenue(), 0, 'f', 2));
            }
        });

        // Initial update
        m_totalOrdersLabel->setText(QString::number(m_orderManager->getTotalOrderCount()));
        m_totalRevenueLabel->setText(QString("$%1").arg(m_orderManager->getTotalRevenue(), 0, 'f', 2));
    }
}

void CRM_Dashboard::setupConnections()
{
    if (m_statusFilter) {
        connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &CRM_Dashboard::filterTable);
    }
    if (m_dateFrom) {
        connect(m_dateFrom, &QDateEdit::dateChanged, this, &CRM_Dashboard::filterTable);
    }
    if (m_dateTo) {
        connect(m_dateTo, &QDateEdit::dateChanged, this, &CRM_Dashboard::filterTable);
    }
}

void CRM_Dashboard::onTabChanged(int index)
{
    if (m_mainTabWidget) {
        QString tabText = m_mainTabWidget->tabText(index);
        statusBar()->showMessage(tr("Switched to %1 tab").arg(tabText), 2000);

        // Update toolbar visibility based on current tab
        if (index == 0) { // Customers tab
            QToolBar *toolbar = findChild<QToolBar*>("ActionsToolBar");
            if (toolbar) toolbar->show();
        } else {
            QToolBar *toolbar = findChild<QToolBar*>("ActionsToolBar");
            if (toolbar) toolbar->hide();
        }
    }
}

void CRM_Dashboard::showCustomerSearch()
{
    // Switch to analytics tab
    if (m_mainTabWidget) {
        m_mainTabWidget->setCurrentIndex(3);
    }
}

void CRM_Dashboard::addCustomer()
{
    CustomerDialog dialog(this, false);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList data = dialog.getCustomerData();
        QList<QStandardItem*> items;
        for (const QString &field : data) {
            items.append(new QStandardItem(field));
        }

        // Color code VIP customers
        if (data[4] == "VIP") {
            items[4]->setBackground(QColor(155, 89, 182));
            items[4]->setForeground(Qt::white);
        }

        m_model->appendRow(items);
        updateCustomerTotals();
        showNotification(tr("Customer Added"), tr("New customer %1 added successfully").arg(data[0]));
    }
}

void CRM_Dashboard::editCustomer()
{
    if (!m_table || !m_proxy || !m_model) return;

    QModelIndex index = m_table->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a customer to edit."));
        return;
    }

    QModelIndex sourceIndex = m_proxy->mapToSource(index);
    int row = sourceIndex.row();

    QStringList currentData;
    for (int col = 0; col < m_model->columnCount(); ++col) {
        currentData << m_model->item(row, col)->text();
    }

    CustomerDialog dialog(this, true);
    dialog.setCustomerData(currentData);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList newData = dialog.getCustomerData();
        for (int col = 0; col < newData.size() && col < m_model->columnCount(); ++col) {
            m_model->item(row, col)->setText(newData[col]);

            // Color code VIP customers
            if (col == 4 && newData[col] == "VIP") {
                m_model->item(row, col)->setBackground(QColor(155, 89, 182));
                m_model->item(row, col)->setForeground(Qt::white);
            } else if (col == 4) {
                m_model->item(row, col)->setBackground(QBrush());
                m_model->item(row, col)->setForeground(QBrush());
            }
        }
        updateCustomerTotals();
        showNotification(tr("Customer Updated"), tr("Customer %1 updated successfully").arg(newData[0]));
    }
}

void CRM_Dashboard::deleteCustomer()
{
    if (!m_table || !m_proxy || !m_model) return;

    QModelIndex index = m_table->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a customer to delete."));
        return;
    }

    QModelIndex sourceIndex = m_proxy->mapToSource(index);
    QString customerName = m_model->item(sourceIndex.row(), 0)->text();

    int ret = QMessageBox::question(this, tr("Confirm Delete"),
                                    tr("Are you sure you want to delete customer '%1'?").arg(customerName),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        m_model->removeRow(sourceIndex.row());
        updateCustomerTotals();
        showNotification(tr("Customer Deleted"), tr("Customer %1 deleted successfully").arg(customerName));
    }
}

void CRM_Dashboard::exportCsv()
{
    if (!m_model) return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Customers"),
                                                    "customers.csv", tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Error"), tr("Could not open file for writing."));
        return;
    }

    QTextStream out(&file);

    // Write headers
    QStringList headers;
    for (int col = 0; col < m_model->columnCount(); ++col) {
        headers << m_model->headerData(col, Qt::Horizontal).toString();
    }
    out << headers.join(",") << "\n";

    // Write data
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_model->columnCount(); ++col) {
            rowData << m_model->item(row, col)->text();
        }
        out << rowData.join(",") << "\n";
    }

    showNotification(tr("Export Complete"), tr("Customers exported to %1").arg(fileName));
}

void CRM_Dashboard::importCsv()
{
    if (!m_model) return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Customers"),
                                                    "", tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Import Error"), tr("Could not open file for reading."));
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine(); // Skip header
    int importCount = 0;

    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() >= 7) {
            QList<QStandardItem*> items;
            for (const QString &field : fields) {
                items.append(new QStandardItem(field.trimmed()));
            }

            // Color code VIP customers
            if (fields[4].trimmed() == "VIP") {
                items[4]->setBackground(QColor(155, 89, 182));
                items[4]->setForeground(Qt::white);
            }

            m_model->appendRow(items);
            importCount++;
        }
    }

    updateCustomerTotals();
    showNotification(tr("Import Complete"), tr("Imported %1 customers").arg(importCount));
}

void CRM_Dashboard::showAbout()
{
    QMessageBox::about(this, tr("About CRM Dashboard"),
                       tr("CRM Dashboard v1.0\n\n"
                          "A comprehensive Customer Relationship Management system.\n\n"
                          "User: %1 (%2)").arg(m_userName, m_userEmail));
}

void CRM_Dashboard::filterTable()
{
    // This is a placeholder for filter functionality
    // You would implement actual filtering logic here based on date range and status
}

void CRM_Dashboard::showNotification(const QString &title, const QString &message)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    } else {
        statusBar()->showMessage(message, 5000);
    }
}

void CRM_Dashboard::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void CRM_Dashboard::createSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_trayIcon->setVisible(true);
}

void CRM_Dashboard::updateStatus()
{
    statusBar()->showMessage(tr("Ready - User: %1").arg(m_userName));
}

void CRM_Dashboard::updateCustomerTotals()
{
    if (!m_model || !m_totalCustomersLabel || !m_totalValueLabel || !m_activeCustomersLabel) {
        return;
    }

    int totalCustomers = m_model->rowCount();
    double totalValue = 0.0;
    int activeCustomers = 0;

    for (int row = 0; row < m_model->rowCount(); ++row) {
        // Calculate total value (column 5)
        QStandardItem *valueItem = m_model->item(row, 5);
        if (valueItem) {
            totalValue += valueItem->text().toDouble();
        }

        // Count active customers (VIP and Customer status)
        QStandardItem *statusItem = m_model->item(row, 4);
        if (statusItem) {
            QString status = statusItem->text();
            if (status == "VIP" || status == "Customer") {
                activeCustomers++;
            }
        }
    }

    m_totalCustomersLabel->setText(QString::number(totalCustomers));
    m_totalValueLabel->setText(QString("$%1").arg(totalValue, 0, 'f', 2));
    m_activeCustomersLabel->setText(QString::number(activeCustomers));
}

void CRM_Dashboard::loadSampleData()
{
    if (!m_model) return;

    m_model->setHorizontalHeaderLabels({
        "Name", "Email", "Phone", "Company", "Status", "Value", "Last Contact"
    });

    QStringList sampleData = {
        "Aditya Darekar,aditya@example.com,555-1111,Tech Solutions,VIP,15000,2024-01-15",
        "Somesh Atole,somesh@example.com,555-2222,Atole Enterprises,Customer,8500,2024-01-20",
        "Saurabh Dharmadhikari,saurabh@example.com,555-3333,Dharma Corp,Lead,5200,2024-01-10",
        "Madan Chitle,madan@example.com,555-4444,Chitle Industries,Customer,12800,2024-01-05",
        "John Smith,john@example.com,555-5555,Smith & Co,Lead,3500,2024-01-18",
        "Sarah Johnson,sarah@example.com,555-6666,Johnson LLC,Customer,9200,2024-01-22",
        "Mike Wilson,mike@example.com,555-7777,Wilson Tech,Prospect,2100,2024-01-12",
        "Emma Davis,emma@example.com,555-8888,Davis Group,VIP,18500,2024-01-25"
    };

    for (const QString &entry : sampleData) {
        QStringList fields = entry.split(',');
        QList<QStandardItem*> items;
        for (const QString &field : fields) {
            items.append(new QStandardItem(field));
        }

        // Color code VIP customers
        if (fields[4] == "VIP") {
            items[4]->setBackground(QColor(155, 89, 182));
            items[4]->setForeground(Qt::white);
        }

        m_model->appendRow(items);
    }

    updateCustomerTotals();
}

void CRM_Dashboard::applyStyleSheet()
{
    QString styleSheet = R"(
        /* Main window - Light background */
        QMainWindow {
            background-color: #ffffff;
            color: #333333;
        }

        /* Tab widget with better contrast */
        QTabWidget::pane {
            border: 1px solid #cccccc;
            background-color: #ffffff;
            border-radius: 4px;
        }

        QTabBar::tab {
            background-color: #f8f9fa;
            color: #495057;
            padding: 12px 24px;
            margin-right: 2px;
            margin-bottom: 2px;
            font-weight: 500;
            border: 1px solid #dee2e6;
            border-bottom: none;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }

        QTabBar::tab:selected {
            background-color: #ffffff;
            color: #212529;
            border-bottom: 2px solid #0066cc;
            font-weight: 600;
        }

        QTabBar::tab:hover:!selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }

        /* Tables with improved visibility */
        QTableView {
            border: 1px solid #dee2e6;
            background-color: #ffffff;
            alternate-background-color: #f8f9fa;
            selection-background-color: #cce5ff;
            selection-color: #000000;
            gridline-color: #e9ecef;
            color: #212529;
        }

        QTableView::item {
            padding: 8px;
            border: none;
        }

        QTableView::item:selected {
            background-color: #0066cc;
            color: #ffffff;
        }

        QHeaderView::section {
            background-color: #e9ecef;
            color: #495057;
            padding: 10px 8px;
            border: none;
            border-right: 1px solid #dee2e6;
            border-bottom: 2px solid #adb5bd;
            font-weight: 600;
            text-align: left;
        }

        QHeaderView::section:hover {
            background-color: #dee2e6;
        }

        /* Buttons with better contrast */
        QPushButton {
            background-color: #0066cc;
            color: #ffffff;
            border: 1px solid #0056b3;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: 500;
            min-height: 20px;
        }

        QPushButton:hover {
            background-color: #0056b3;
            border-color: #004085;
        }

        QPushButton:pressed {
            background-color: #004085;
            border-color: #003266;
        }

        QPushButton:disabled {
            background-color: #6c757d;
            border-color: #6c757d;
            color: #ffffff;
        }

        /* Secondary button style */
        QPushButton[class="secondary"] {
            background-color: #6c757d;
            border-color: #5a6268;
        }

        QPushButton[class="secondary"]:hover {
            background-color: #5a6268;
            border-color: #495057;
        }

        /* Input fields */
        QLineEdit, QComboBox, QDateEdit, QSpinBox, QTextEdit {
            padding: 8px 12px;
            border: 1px solid #ced4da;
            border-radius: 4px;
            background-color: #ffffff;
            color: #495057;
            selection-background-color: #0066cc;
            selection-color: #ffffff;
        }

        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QSpinBox:focus, QTextEdit:focus {
            border-color: #0066cc;
            outline: none;
            box-shadow: 0 0 0 2px rgba(0, 102, 204, 0.25);
        }

        QLineEdit:disabled, QComboBox:disabled, QDateEdit:disabled, QSpinBox:disabled {
            background-color: #e9ecef;
            color: #6c757d;
            border-color: #dee2e6;
        }

        /* ComboBox dropdown */
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }

        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #6c757d;
            margin-right: 5px;
        }

        QComboBox QAbstractItemView {
            border: 1px solid #ced4da;
            background-color: #ffffff;
            selection-background-color: #0066cc;
            selection-color: #ffffff;
        }

        /* Group boxes */
        QGroupBox {
            font-weight: 600;
            color: #495057;
            border: 2px solid #dee2e6;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 12px;
            background-color: #ffffff;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
            background-color: #ffffff;
            color: #212529;
        }

        /* Labels */
        QLabel {
            color: #495057;
        }

        /* Dock widgets */
        QDockWidget {
            color: #495057;
            background-color: #f8f9fa;
        }

        QDockWidget::title {
            background-color: #e9ecef;
            color: #495057;
            padding: 8px;
            border-bottom: 1px solid #dee2e6;
            font-weight: 600;
        }

        /* Status bar */
        QStatusBar {
            background-color: #f8f9fa;
            color: #495057;
            border-top: 1px solid #dee2e6;
        }

        /* Toolbar with better icon visibility */
        QToolBar {
            background-color: #ffffff;
            border: 1px solid #dee2e6;
            border-left: none;
            border-right: none;
            padding: 4px;
            spacing: 4px;
        }

        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 4px;
            padding: 6px;
            margin: 2px;
            color: #495057;
            min-width: 60px;
            min-height: 60px;
        }

        QToolButton:hover {
            background-color: #e3f2fd;
            border-color: #bbdefb;
            color: #1976d2;
        }

        QToolButton:pressed {
            background-color: #bbdefb;
            border-color: #90caf9;
            color: #0d47a1;
        }

        QToolButton:checked {
            background-color: #0066cc;
            border-color: #0056b3;
            color: #ffffff;
        }

        QToolButton:disabled {
            background-color: transparent;
            border-color: transparent;
            color: #adb5bd;
        }

        /* Toolbar separator */
        QToolBar::separator {
            background-color: #dee2e6;
            width: 1px;
            margin: 4px 2px;
        }

        /* List widgets */
        QListWidget {
            border: 1px solid #dee2e6;
            background-color: #ffffff;
            color: #495057;
            selection-background-color: #0066cc;
            selection-color: #ffffff;
        }

        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #f8f9fa;
        }

        QListWidget::item:hover {
            background-color: #f8f9fa;
        }

        QListWidget::item:selected {
            background-color: #0066cc;
            color: #ffffff;
        }

        /* Scroll bars */
        QScrollBar:vertical {
            background-color: #f8f9fa;
            width: 12px;
            border: none;
        }

        QScrollBar::handle:vertical {
            background-color: #ced4da;
            min-height: 20px;
            border-radius: 6px;
            margin: 2px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #adb5bd;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background-color: #f8f9fa;
            height: 12px;
            border: none;
        }

        QScrollBar::handle:horizontal {
            background-color: #ced4da;
            min-width: 20px;
            border-radius: 6px;
            margin: 2px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #adb5bd;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* Dialog boxes */
        QDialog {
            background-color: #ffffff;
            color: #495057;
        }

        /* Message boxes */
        QMessageBox {
            background-color: #ffffff;
            color: #495057;
        }

        QMessageBox QLabel {
            color: #495057;
        }

        /* Progress bars */
        QProgressBar {
            border: 1px solid #dee2e6;
            border-radius: 4px;
            text-align: center;
            background-color: #f8f9fa;
            color: #495057;
        }

        QProgressBar::chunk {
            background-color: #28a745;
            border-radius: 3px;
        }

        /* Menu bar */
        QMenuBar {
            background-color: #ffffff;
            color: #495057;
            border-bottom: 1px solid #dee2e6;
        }

        QMenuBar::item {
            background-color: transparent;
            padding: 8px 12px;
        }

        QMenuBar::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }

        QMenu {
            background-color: #ffffff;
            color: #495057;
            border: 1px solid #dee2e6;
        }

        QMenu::item {
            padding: 8px 20px;
        }

        QMenu::item:selected {
            background-color: #0066cc;
            color: #ffffff;
        }

        /* Splitter */
        QSplitter::handle {
            background-color: #dee2e6;
        }

        QSplitter::handle:horizontal {
            width: 3px;
        }

        QSplitter::handle:vertical {
            height: 3px;
        }
    )";

    qApp->setStyleSheet(styleSheet);
}
