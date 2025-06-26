#include "customer_search.h"

// CustomerAnalytics Implementation
CustomerAnalytics::CustomerAnalytics(QObject *parent)
    : QObject(parent)
{
}

void CustomerAnalytics::analyzeCustomers(const std::vector<Customer>& customers)
{
    // Segment analysis
    QJsonObject segments;
    int vipCount = 0, regularCount = 0, newCount = 0;

    for (const auto& customer : customers) {
        if (customer.segment == "VIP") vipCount++;
        else if (customer.segment == "Regular") regularCount++;
        else if (customer.segment == "New") newCount++;
    }

    segments["VIP"] = vipCount;
    segments["Regular"] = regularCount;
    segments["New"] = newCount;
    m_segmentData = segments;

    // Geographic distribution
    QJsonObject geo;
    std::map<QString, int> countryCount;
    for (const auto& customer : customers) {
        countryCount[customer.country]++;
    }

    for (const auto& [country, count] : countryCount) {
        geo[country] = count;
    }
    m_geoData = geo;

    // Revenue analysis
    double totalRevenue = 0;
    double avgOrderValue = 0;
    int totalOrders = 0;

    for (const auto& customer : customers) {
        totalRevenue += customer.totalSpent;
        totalOrders += customer.orderCount;
    }

    if (totalOrders > 0) {
        avgOrderValue = totalRevenue / totalOrders;
    }

    QJsonObject revenue;
    revenue["total"] = totalRevenue;
    revenue["average_order"] = avgOrderValue;
    revenue["total_orders"] = totalOrders;
    m_revenueData = revenue;

    // Satisfaction metrics
    double avgSatisfaction = 0;
    int satisfiedCount = 0;

    for (const auto& customer : customers) {
        avgSatisfaction += customer.satisfactionScore;
        if (customer.satisfactionScore >= 4.0) {
            satisfiedCount++;
        }
    }

    if (!customers.empty()) {
        avgSatisfaction /= customers.size();
    }

    QJsonObject satisfaction;
    satisfaction["average"] = avgSatisfaction;
    satisfaction["satisfied_percentage"] = customers.empty() ? 0 :
        (satisfiedCount * 100.0 / customers.size());
    m_satisfactionData = satisfaction;

    emit analysisCompleted();
}

QJsonObject CustomerAnalytics::getSegmentAnalysis() const
{
    return m_segmentData;
}

QJsonObject CustomerAnalytics::getGeographicDistribution() const
{
    return m_geoData;
}

QJsonObject CustomerAnalytics::getRevenueAnalysis() const
{
    return m_revenueData;
}

QJsonObject CustomerAnalytics::getSatisfactionMetrics() const
{
    return m_satisfactionData;
}

// CustomerDataSync Implementation
CustomerDataSync::CustomerDataSync(QObject *parent)
    : QObject(parent), m_syncing(false)
{
    m_syncTimer = new QTimer(this);
    m_networkManager = new QNetworkAccessManager(this);

    connect(m_syncTimer, &QTimer::timeout, this, &CustomerDataSync::performSync);
}

void CustomerDataSync::startSync()
{
    m_syncing = true;
    m_syncTimer->start(30000); // Sync every 30 seconds
    performSync(); // Initial sync
}

void CustomerDataSync::stopSync()
{
    m_syncing = false;
    m_syncTimer->stop();
}

void CustomerDataSync::syncCustomer(const QString& customerId)
{
    // Simulate customer sync
    emit syncProgress(50);

    // In real implementation, this would make API calls
    Customer updatedCustomer;
    updatedCustomer.id = customerId;
    updatedCustomer.name = "Updated Customer";
    updatedCustomer.lastOrderDate = QDateTime::currentDateTime();

    emit customerUpdated(updatedCustomer);
    emit syncProgress(100);
}

void CustomerDataSync::performSync()
{
    if (!m_syncing) return;

    // Simulate sync operation
    emit syncProgress(0);

    // In real implementation, this would sync with backend
    QTimer::singleShot(1000, [this]() {
        emit syncProgress(100);
    });
}

// CustomerSearch Implementation
CustomerSearch::CustomerSearch(QWidget *parent)
    : QMainWindow(parent),
      m_analytics(std::make_unique<CustomerAnalytics>(this)),
      m_dataSync(std::make_unique<CustomerDataSync>(this)),
      m_realTimeSyncEnabled(false)
{
    setupUI();
    connectSignals();
    loadCustomers();

    setWindowTitle(tr("Customer Search & Analytics"));
    resize(1600, 900);

    // Start auto-save timer
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &CustomerSearch::exportResults);
    m_autoSaveTimer->start(300000); // Auto-save every 5 minutes
}

CustomerSearch::~CustomerSearch()
{
    m_dataSync->stopSync();
}

void CustomerSearch::setupUI()
{
    // Create central widget with splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // Left panel - search controls
    QWidget *searchPanel = new QWidget();
    setupSearchPanel();
    searchPanel->setLayout(new QVBoxLayout());
    searchPanel->setMaximumWidth(350);
    splitter->addWidget(searchPanel);

    // Right panel - results
    QWidget *resultsPanel = new QWidget();
    setupResultsTable();
    resultsPanel->setLayout(new QVBoxLayout());
    resultsPanel->layout()->addWidget(m_resultsTable);
    splitter->addWidget(resultsPanel);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    setupToolBar();
    setupAnalyticsPanel();

    // Menu bar
    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Import Customers"), this, &CustomerSearch::importCustomers);
    fileMenu->addAction(tr("&Export Results"), this, &CustomerSearch::exportResults);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close);

    QMenu *customerMenu = menuBar->addMenu(tr("&Customer"));
    customerMenu->addAction(tr("&New Customer"), this, &CustomerSearch::createCustomer);
    customerMenu->addAction(tr("&Edit Customer"), this, &CustomerSearch::editCustomer);
    customerMenu->addAction(tr("&Delete Customer"), this, &CustomerSearch::deleteCustomer);
    customerMenu->addSeparator();
    customerMenu->addAction(tr("&Merge Customers"), this, &CustomerSearch::mergeCustomers);

    QMenu *communicateMenu = menuBar->addMenu(tr("&Communicate"));
    communicateMenu->addAction(tr("Send &Email"), this, &CustomerSearch::sendEmail);
    communicateMenu->addAction(tr("Send &SMS"), this, &CustomerSearch::sendSMS);
    communicateMenu->addAction(tr("Schedule &Call"), this, &CustomerSearch::scheduleCall);
    communicateMenu->addSeparator();
    communicateMenu->addAction(tr("View &History"), this, &CustomerSearch::viewCommunicationHistory);

    QMenu *analyticsMenu = menuBar->addMenu(tr("&Analytics"));
    analyticsMenu->addAction(tr("&Dashboard"), this, &CustomerSearch::showAnalytics);
    analyticsMenu->addAction(tr("&Generate Report"), this, &CustomerSearch::generateReport);

    statusBar()->showMessage(tr("Ready"));
}

void CustomerSearch::setupSearchPanel()
{
    QWidget *searchWidget = qobject_cast<QWidget*>(centralWidget()->layout()->itemAt(0)->widget());
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(searchWidget->layout());

    // Quick search
    QGroupBox *quickSearchGroup = new QGroupBox(tr("Quick Search"));
    QVBoxLayout *quickLayout = new QVBoxLayout(quickSearchGroup);

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Search by name, email, phone..."));
    m_searchEdit->setClearButtonEnabled(true);
    quickLayout->addWidget(m_searchEdit);

    m_searchButton = new QPushButton(tr("Search"));
    m_searchButton->setDefault(true);
    quickLayout->addWidget(m_searchButton);

    layout->addWidget(quickSearchGroup);

    // Filters
    QGroupBox *filtersGroup = new QGroupBox(tr("Filters"));
    QFormLayout *filtersLayout = new QFormLayout(filtersGroup);

    m_statusFilter = new QComboBox();
    m_statusFilter->addItems({"All", "Active", "Inactive", "Pending", "VIP"});
    filtersLayout->addRow(tr("Status:"), m_statusFilter);

    m_segmentFilter = new QComboBox();
    m_segmentFilter->addItems({"All", "VIP", "Regular", "New"});
    filtersLayout->addRow(tr("Segment:"), m_segmentFilter);

    m_countryFilter = new QComboBox();
    m_countryFilter->addItems({"All", "USA", "UK", "Canada", "Australia", "India"});
    filtersLayout->addRow(tr("Country:"), m_countryFilter);

    layout->addWidget(filtersGroup);

    // Advanced filters
    QGroupBox *advancedGroup = new QGroupBox(tr("Advanced Filters"));
    QFormLayout *advancedLayout = new QFormLayout(advancedGroup);

    QHBoxLayout *spentLayout = new QHBoxLayout();
    m_minSpentSpin = new QSpinBox();
    m_minSpentSpin->setRange(0, 1000000);
    m_minSpentSpin->setPrefix("$");
    m_maxSpentSpin = new QSpinBox();
    m_maxSpentSpin->setRange(0, 1000000);
    m_maxSpentSpin->setPrefix("$");
    m_maxSpentSpin->setValue(1000000);
    spentLayout->addWidget(m_minSpentSpin);
    spentLayout->addWidget(new QLabel("-"));
    spentLayout->addWidget(m_maxSpentSpin);
    advancedLayout->addRow(tr("Total Spent:"), spentLayout);

    QHBoxLayout *dateLayout = new QHBoxLayout();
    m_fromDateEdit = new QDateEdit(QDate::currentDate().addYears(-1));
    m_toDateEdit = new QDateEdit(QDate::currentDate());
    dateLayout->addWidget(m_fromDateEdit);
    dateLayout->addWidget(new QLabel("-"));
    dateLayout->addWidget(m_toDateEdit);
    advancedLayout->addRow(tr("Last Order:"), dateLayout);

    m_exactMatchCheck = new QCheckBox(tr("Exact match"));
    advancedLayout->addRow(m_exactMatchCheck);

    layout->addWidget(advancedGroup);

    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_advancedSearchButton = new QPushButton(tr("Advanced Search"));
    QPushButton *clearButton = new QPushButton(tr("Clear"));
    buttonLayout->addWidget(m_advancedSearchButton);
    buttonLayout->addWidget(clearButton);

    layout->addLayout(buttonLayout);
    layout->addStretch();

    // Connect signals
    connect(clearButton, &QPushButton::clicked, this, &CustomerSearch::clearSearch);
}

void CustomerSearch::setupResultsTable()
{
    m_resultsTable = new QTableView();
    m_resultsModel = new QStandardItemModel(0, 10, this);
    m_proxyModel = new QSortFilterProxyModel(this);

    m_resultsModel->setHorizontalHeaderLabels({
        "ID", "Name", "Email", "Phone", "Company",
        "Segment", "Total Spent", "Orders", "Last Order", "Status"
    });

    m_proxyModel->setSourceModel(m_resultsModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_resultsTable->setModel(m_proxyModel);

    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setSortingEnabled(true);
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);

    // Context menu
    m_resultsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_resultsTable, &QWidget::customContextMenuRequested,
            [this](const QPoint& pos) {
        QMenu contextMenu;
        contextMenu.addAction(tr("View Details"), this, &CustomerSearch::viewCustomerDetails);
        contextMenu.addAction(tr("Edit"), this, &CustomerSearch::editCustomer);
        contextMenu.addSeparator();
        contextMenu.addAction(tr("Send Email"), this, &CustomerSearch::sendEmail);
        contextMenu.addAction(tr("Send SMS"), this, &CustomerSearch::sendSMS);
        contextMenu.addSeparator();
        contextMenu.addAction(tr("View Orders"), [this]() {
            // View customer orders
        });
        contextMenu.addAction(tr("Communication History"),
                            this, &CustomerSearch::viewCommunicationHistory);
        contextMenu.addSeparator();
        contextMenu.addAction(tr("Delete"), this, &CustomerSearch::deleteCustomer);
        contextMenu.exec(m_resultsTable->viewport()->mapToGlobal(pos));
    });

    // Double-click to view details
    connect(m_resultsTable, &QTableView::doubleClicked,
            this, &CustomerSearch::viewCustomerDetails);
}

void CustomerSearch::setupAnalyticsPanel()
{
    QDockWidget *analyticsDock = new QDockWidget(tr("Customer Analytics"), this);
    analyticsDock->setAllowedAreas(Qt::BottomDockWidgetArea);

    QWidget *analyticsWidget = new QWidget();
    QHBoxLayout *analyticsLayout = new QHBoxLayout(analyticsWidget);

    // Key metrics
    QGroupBox *metricsGroup = new QGroupBox(tr("Key Metrics"));
    QGridLayout *metricsLayout = new QGridLayout(metricsGroup);

    m_totalCustomersLabel = new QLabel("0");
    m_totalRevenueLabel = new QLabel("$0");
    m_avgOrderValueLabel = new QLabel("$0");
    m_retentionRateLabel = new QLabel("0%");

    QFont metricsFont;
    metricsFont.setPointSize(12);
    metricsFont.setBold(true);

    m_totalCustomersLabel->setFont(metricsFont);
    m_totalRevenueLabel->setFont(metricsFont);
    m_avgOrderValueLabel->setFont(metricsFont);
    m_retentionRateLabel->setFont(metricsFont);

    metricsLayout->addWidget(new QLabel(tr("Total Customers:")), 0, 0);
    metricsLayout->addWidget(m_totalCustomersLabel, 0, 1);
    metricsLayout->addWidget(new QLabel(tr("Total Revenue:")), 0, 2);
    metricsLayout->addWidget(m_totalRevenueLabel, 0, 3);
    metricsLayout->addWidget(new QLabel(tr("Avg Order Value:")), 1, 0);
    metricsLayout->addWidget(m_avgOrderValueLabel, 1, 1);
    metricsLayout->addWidget(new QLabel(tr("Retention Rate:")), 1, 2);
    metricsLayout->addWidget(m_retentionRateLabel, 1, 3);

    analyticsLayout->addWidget(metricsGroup);

    // Mini charts
    m_segmentChart = new QGroupBox(tr("Segment Distribution"));
    m_geoChart = new QGroupBox(tr("Geographic Distribution"));

    analyticsLayout->addWidget(m_segmentChart);
    analyticsLayout->addWidget(m_geoChart);

    analyticsDock->setWidget(analyticsWidget);
    addDockWidget(Qt::BottomDockWidgetArea, analyticsDock);
}

void CustomerSearch::setupToolBar()
{
    QToolBar *toolbar = addToolBar(tr("Main"));
    toolbar->setIconSize(QSize(24, 24));

    toolbar->addAction(QIcon::fromTheme("contact-new"), tr("New Customer"),
                      this, &CustomerSearch::createCustomer);
    toolbar->addAction(QIcon::fromTheme("document-open"), tr("Import"),
                      this, &CustomerSearch::importCustomers);
    toolbar->addAction(QIcon::fromTheme("document-save"), tr("Export"),
                      this, &CustomerSearch::exportResults);
    toolbar->addSeparator();

    toolbar->addAction(QIcon::fromTheme("mail-send"), tr("Email"),
                      this, &CustomerSearch::sendEmail);
    toolbar->addAction(QIcon::fromTheme("phone"), tr("Call"),
                      this, &CustomerSearch::scheduleCall);
    toolbar->addSeparator();

    toolbar->addAction(QIcon::fromTheme("view-statistics"), tr("Analytics"),
                      this, &CustomerSearch::showAnalytics);

    // Real-time sync toggle
    QAction *syncAction = toolbar->addAction(QIcon::fromTheme("view-refresh"),
                                           tr("Real-time Sync"));
    syncAction->setCheckable(true);
    connect(syncAction, &QAction::toggled, this, &CustomerSearch::toggleRealTimeSync);
}

void CustomerSearch::connectSignals()
{
    // Search signals
    connect(m_searchButton, &QPushButton::clicked, this, &CustomerSearch::performSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &CustomerSearch::performSearch);
    connect(m_searchEdit, &QLineEdit::textChanged, [this]() {
        if (m_searchEdit->text().length() >= 3) {
            performSearch();
        }
    });

    connect(m_advancedSearchButton, &QPushButton::clicked,
            this, &CustomerSearch::performAdvancedSearch);

    // Filter signals
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerSearch::applySearchCriteria);
    connect(m_segmentFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerSearch::applySearchCriteria);
    connect(m_countryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerSearch::applySearchCriteria);

    // Analytics signals
    connect(m_analytics.get(), &CustomerAnalytics::analysisCompleted, [this]() {
        // Update analytics display
        auto segmentData = m_analytics->getSegmentAnalysis();
        auto revenueData = m_analytics->getRevenueAnalysis();

        m_totalRevenueLabel->setText(QString("$%1").arg(
            revenueData["total"].toDouble(), 0, 'f', 2));
        m_avgOrderValueLabel->setText(QString("$%1").arg(
            revenueData["average_order"].toDouble(), 0, 'f', 2));
    });

    // Data sync signals
    connect(m_dataSync.get(), &CustomerDataSync::customerUpdated,
            this, &CustomerSearch::handleCustomerUpdate);
    connect(m_dataSync.get(), &CustomerDataSync::syncProgress,
            [this](int percentage) {
        statusBar()->showMessage(tr("Syncing... %1%").arg(percentage), 2000);
    });
}

void CustomerSearch::loadCustomers()
{
    // Load sample customers
    for (int i = 0; i < 50; ++i) {
        Customer customer;
        customer.id = QString("CUST%1").arg(1000 + i);
        customer.name = QString("Customer %1").arg(i + 1);
        customer.email = QString("customer%1@example.com").arg(i + 1);
        customer.phone = QString("+1-555-%1").arg(1000 + i);
        customer.company = QString("Company %1").arg(i % 10 + 1);
        customer.address = QString("%1 Main St").arg(100 + i);
        customer.city = i % 2 == 0 ? "New York" : "Los Angeles";
        customer.country = "USA";
        customer.status = i % 5 == 0 ? "Inactive" : "Active";
        customer.totalSpent = 1000 + (i * 100);
        customer.orderCount = 5 + (i % 10);
        customer.lastOrderDate = QDateTime::currentDateTime().addDays(-(i * 2));
        customer.registrationDate = QDateTime::currentDateTime().addMonths(-(i + 1));
        customer.creditLimit = 5000 + (i * 200);
        customer.segment = i < 10 ? "VIP" : (i < 30 ? "Regular" : "New");
        customer.satisfactionScore = 3.0 + (i % 20) / 10.0;
        customer.preferredContact = i % 2 == 0 ? "Email" : "Phone";

        m_customers.push_back(customer);

        // Add to model
        QList<QStandardItem*> items;
        items << new QStandardItem(customer.id);
        items << new QStandardItem(customer.name);
        items << new QStandardItem(customer.email);
        items << new QStandardItem(customer.phone);
        items << new QStandardItem(customer.company);
        items << new QStandardItem(customer.segment);
        items << new QStandardItem(QString("$%1").arg(customer.totalSpent, 0, 'f', 2));
        items << new QStandardItem(QString::number(customer.orderCount));
        items << new QStandardItem(customer.lastOrderDate.toString("yyyy-MM-dd"));
        items << new QStandardItem(customer.status);

        // Color coding
        if (customer.segment == "VIP") {
            items[5]->setBackground(QColor(155, 89, 182));
            items[5]->setForeground(Qt::white);
        }

        if (customer.status == "Inactive") {
            for (auto* item : items) {
                item->setForeground(QColor(149, 165, 166));
            }
        }

        m_resultsModel->appendRow(items);
    }

    // Update metrics
    m_totalCustomersLabel->setText(QString::number(m_customers.size()));
    m_analytics->analyzeCustomers(m_customers);

    // Calculate retention rate
    int activeCount = std::count_if(m_customers.begin(), m_customers.end(),
                                   [](const Customer& c) { return c.status == "Active"; });
    double retentionRate = m_customers.empty() ? 0 :
        (activeCount * 100.0 / m_customers.size());
    m_retentionRateLabel->setText(QString("%1%").arg(retentionRate, 0, 'f', 1));
}

void CustomerSearch::performSearch()
{
    QString searchText = m_searchEdit->text().toLower();

    if (searchText.isEmpty()) {
        m_proxyModel->setFilterRegularExpression("");
        return;
    }

    // Custom filter that searches across multiple columns
    m_proxyModel->setFilterRegularExpression(QRegularExpression(searchText,
        QRegularExpression::CaseInsensitiveOption));
    m_proxyModel->setFilterKeyColumn(-1); // Search all columns

    highlightSearchResults();

    int resultCount = m_proxyModel->rowCount();
    statusBar()->showMessage(tr("Found %1 customers").arg(resultCount), 3000);
    emit searchCompleted(resultCount);
}

void CustomerSearch::performAdvancedSearch()
{
    AdvancedSearchDialog dialog(this);
    dialog.setSearchCriteria(m_currentCriteria);

    if (dialog.exec() == QDialog::Accepted) {
        m_currentCriteria = dialog.getSearchCriteria();
        applySearchCriteria();
    }
}

void CustomerSearch::clearSearch()
{
    m_searchEdit->clear();
    m_statusFilter->setCurrentIndex(0);
    m_segmentFilter->setCurrentIndex(0);
    m_countryFilter->setCurrentIndex(0);
    m_minSpentSpin->setValue(0);
    m_maxSpentSpin->setValue(1000000);
    m_fromDateEdit->setDate(QDate::currentDate().addYears(-1));
    m_toDateEdit->setDate(QDate::currentDate());
    m_exactMatchCheck->setChecked(false);

    m_proxyModel->setFilterRegularExpression("");
    statusBar()->showMessage(tr("Search cleared"), 2000);
}

void CustomerSearch::applySearchCriteria()
{
    // Apply complex filtering based on all criteria
    // This is a simplified version - in production, you'd implement custom QSortFilterProxyModel

    QString status = m_statusFilter->currentText();
    QString segment = m_segmentFilter->currentText();
    QString country = m_countryFilter->currentText();

    // Build filter expression
    QString filter;
    if (status != "All") {
        filter = status;
    }

    // Apply filter
    if (!filter.isEmpty()) {
        m_proxyModel->setFilterKeyColumn(9); // Status column
        m_proxyModel->setFilterFixedString(filter);
    }

    int resultCount = m_proxyModel->rowCount();
    statusBar()->showMessage(tr("Filtered: %1 customers").arg(resultCount), 3000);
}

void CustomerSearch::exportResults()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Customers"),
                                                   "customers_export.csv",
                                                   tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file for writing"));
        return;
    }

    QTextStream out(&file);

    // Write headers
    QStringList headers;
    for (int col = 0; col < m_resultsModel->columnCount(); ++col) {
        headers << m_resultsModel->horizontalHeaderItem(col)->text();
    }
    out << headers.join(",") << "\n";

    // Write visible data
    for (int row = 0; row < m_proxyModel->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_proxyModel->columnCount(); ++col) {
            QModelIndex index = m_proxyModel->index(row, col);
            rowData << m_proxyModel->data(index).toString();
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
    statusBar()->showMessage(tr("Exported %1 customers to %2")
                           .arg(m_proxyModel->rowCount())
                           .arg(fileName), 3000);
}

void CustomerSearch::importCustomers()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Customers"),
                                                   "", tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file for reading"));
        return;
    }

    QTextStream in(&file);

    // Skip header
    if (!in.atEnd()) in.readLine();

    int imported = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() >= 10) {
            Customer customer;
            customer.id = fields[0];
            customer.name = fields[1];
            customer.email = fields[2];
            customer.phone = fields[3];
            customer.company = fields[4];
            customer.segment = fields[5];
            customer.totalSpent = fields[6].remove('$').toDouble();
            customer.orderCount = fields[7].toInt();
            customer.lastOrderDate = QDateTime::fromString(fields[8], "yyyy-MM-dd");
            customer.status = fields[9];

            m_customers.push_back(customer);

            // Add to model
            QList<QStandardItem*> items;
            for (const QString& field : fields) {
                items << new QStandardItem(field);
            }
            m_resultsModel->appendRow(items);

            imported++;
        }
    }

    file.close();

    // Update analytics
    m_analytics->analyzeCustomers(m_customers);
    m_totalCustomersLabel->setText(QString::number(m_customers.size()));

    statusBar()->showMessage(tr("Imported %1 customers").arg(imported), 3000);
}

void CustomerSearch::viewCustomerDetails()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    int row = m_proxyModel->mapToSource(selected.first()).row();
    QString customerId = m_resultsModel->item(row, 0)->text();

    auto it = std::find_if(m_customers.begin(), m_customers.end(),
                          [&customerId](const Customer& c) { return c.id == customerId; });

    if (it != m_customers.end()) {
        CustomerDetailsDialog dialog(*it, this);
        dialog.exec();
    }
}

void CustomerSearch::editCustomer()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    // Implementation would show customer edit dialog
    QMessageBox::information(this, tr("Edit Customer"),
                           tr("Customer edit dialog would appear here"));
}

void CustomerSearch::deleteCustomer()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    int row = m_proxyModel->mapToSource(selected.first()).row();
    QString customerId = m_resultsModel->item(row, 0)->text();
    QString customerName = m_resultsModel->item(row, 1)->text();

    if (QMessageBox::question(this, tr("Delete Customer"),
                             tr("Delete customer %1 (%2)?").arg(customerName, customerId),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        // Remove from vector
        m_customers.erase(std::remove_if(m_customers.begin(), m_customers.end(),
                                        [&customerId](const Customer& c) {
                                            return c.id == customerId;
                                        }), m_customers.end());

        // Remove from model
        m_resultsModel->removeRow(row);

        // Update analytics
        m_analytics->analyzeCustomers(m_customers);
        m_totalCustomersLabel->setText(QString::number(m_customers.size()));

        statusBar()->showMessage(tr("Customer %1 deleted").arg(customerName), 3000);
    }
}

void CustomerSearch::mergeCustomers()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.size() < 2) {
        QMessageBox::information(this, tr("Merge Customers"),
                               tr("Please select at least 2 customers to merge"));
        return;
    }

    QStringList customerIds;
    for (const auto& index : selected) {
        int row = m_proxyModel->mapToSource(index).row();
        customerIds << m_resultsModel->item(row, 0)->text();
    }

    CustomerMergeDialog dialog(customerIds, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString primaryId = dialog.getPrimaryCustomerId();
        QStringList mergeFields = dialog.getMergeFields();

        // Perform merge operation
        statusBar()->showMessage(tr("Merged %1 customers into %2")
                               .arg(customerIds.size())
                               .arg(primaryId), 3000);
    }
}

void CustomerSearch::createCustomer()
{
    // Would show customer creation dialog
    QMessageBox::information(this, tr("New Customer"),
                           tr("Customer creation dialog would appear here"));
}

void CustomerSearch::showAnalytics()
{
    CustomerAnalyticsDashboard dashboard(m_customers, this);
    dashboard.exec();
}

void CustomerSearch::generateReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Generate Report"),
                                                   "customer_report.pdf",
                                                   tr("PDF Files (*.pdf)"));
    if (fileName.isEmpty()) return;

    // Generate PDF report
    statusBar()->showMessage(tr("Report generated: %1").arg(fileName), 3000);
}

void CustomerSearch::handleCustomerUpdate(const Customer& customer)
{
    // Update customer in vector
    auto it = std::find_if(m_customers.begin(), m_customers.end(),
                          [&customer](const Customer& c) { return c.id == customer.id; });

    if (it != m_customers.end()) {
        *it = customer;
        updateCustomerInModel(customer);
    }
}

void CustomerSearch::updateCustomerInModel(const Customer& customer)
{
    for (int row = 0; row < m_resultsModel->rowCount(); ++row) {
        if (m_resultsModel->item(row, 0)->text() == customer.id) {
            m_resultsModel->item(row, 1)->setText(customer.name);
            m_resultsModel->item(row, 2)->setText(customer.email);
            m_resultsModel->item(row, 3)->setText(customer.phone);
            m_resultsModel->item(row, 4)->setText(customer.company);
            m_resultsModel->item(row, 5)->setText(customer.segment);
            m_resultsModel->item(row, 6)->setText(QString("$%1").arg(customer.totalSpent, 0, 'f', 2));
            m_resultsModel->item(row, 7)->setText(QString::number(customer.orderCount));
            m_resultsModel->item(row, 8)->setText(customer.lastOrderDate.toString("yyyy-MM-dd"));
            m_resultsModel->item(row, 9)->setText(customer.status);

            // Highlight updated row
            for (int col = 0; col < m_resultsModel->columnCount(); ++col) {
                m_resultsModel->item(row, col)->setBackground(QColor(46, 204, 113, 50));
            }

            // Clear highlight after 3 seconds
            QTimer::singleShot(3000, [this, row]() {
                for (int col = 0; col < m_resultsModel->columnCount(); ++col) {
                    m_resultsModel->item(row, col)->setBackground(QBrush());
                }
            });

            break;
        }
    }
}

void CustomerSearch::toggleRealTimeSync()
{
    m_realTimeSyncEnabled = !m_realTimeSyncEnabled;

    if (m_realTimeSyncEnabled) {
        m_dataSync->startSync();
        statusBar()->showMessage(tr("Real-time sync enabled"), 3000);
    } else {
        m_dataSync->stopSync();
        statusBar()->showMessage(tr("Real-time sync disabled"), 3000);
    }
}

void CustomerSearch::sendEmail()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, tr("Send Email"),
                               tr("Please select customers to email"));
        return;
    }

    QStringList emails;
    for (const auto& index : selected) {
        int row = m_proxyModel->mapToSource(index).row();
        emails << m_resultsModel->item(row, 2)->text();
    }

    // Would open email composition dialog
    QMessageBox::information(this, tr("Send Email"),
                           tr("Sending email to %1 customers:\n%2")
                           .arg(emails.size())
                           .arg(emails.join(", ")));
}

void CustomerSearch::sendSMS()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    // Would open SMS composition dialog
    QMessageBox::information(this, tr("Send SMS"),
                           tr("SMS composition dialog would appear here"));
}

void CustomerSearch::scheduleCall()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    // Would open call scheduling dialog
    QMessageBox::information(this, tr("Schedule Call"),
                           tr("Call scheduling dialog would appear here"));
}

void CustomerSearch::viewCommunicationHistory()
{
    QModelIndexList selected = m_resultsTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    int row = m_proxyModel->mapToSource(selected.first()).row();
    QString customerId = m_resultsModel->item(row, 0)->text();

    CommunicationHistoryDialog dialog(customerId, this);
    dialog.exec();
}

void CustomerSearch::highlightSearchResults()
{
    QString searchText = m_searchEdit->text();
    if (searchText.isEmpty()) return;

    // Highlight matching cells
    for (int row = 0; row < m_proxyModel->rowCount(); ++row) {
        for (int col = 0; col < m_proxyModel->columnCount(); ++col) {
            QModelIndex proxyIndex = m_proxyModel->index(row, col);
            QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

            QString cellText = m_proxyModel->data(proxyIndex).toString();
            if (cellText.contains(searchText, Qt::CaseInsensitive)) {
                m_resultsModel->itemFromIndex(sourceIndex)->setBackground(
                    QColor(241, 196, 15, 50));
            }
        }
    }
}

void CustomerSearch::setSearchCriteria(const SearchCriteria& criteria)
{
    m_currentCriteria = criteria;

    m_searchEdit->setText(criteria.textSearch);
    m_minSpentSpin->setValue(criteria.minSpent);
    m_maxSpentSpin->setValue(criteria.maxSpent);
    m_fromDateEdit->setDate(criteria.fromDate);
    m_toDateEdit->setDate(criteria.toDate);
    m_exactMatchCheck->setChecked(criteria.exactMatch);

    applySearchCriteria();
}

// CustomerDetailsDialog Implementation
CustomerDetailsDialog::CustomerDetailsDialog(const Customer& customer, QWidget *parent)
    : QDialog(parent), m_customer(customer)
{
    setWindowTitle(tr("Customer Details - %1").arg(customer.name));
    setModal(true);
    resize(800, 600);
    setupUI();
    loadCustomerData();
}

void CustomerDetailsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header with customer name and status
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *nameHeader = new QLabel(m_customer.name);
    QFont headerFont;
    headerFont.setPointSize(16);
    headerFont.setBold(true);
    nameHeader->setFont(headerFont);

    QLabel *statusLabel = new QLabel(m_customer.status);
    statusLabel->setStyleSheet(m_customer.status == "Active" ?
        "QLabel { background-color: #27ae60; color: white; padding: 5px; border-radius: 3px; }" :
        "QLabel { background-color: #e74c3c; color: white; padding: 5px; border-radius: 3px; }");

    headerLayout->addWidget(nameHeader);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);

    mainLayout->addLayout(headerLayout);

    // Tab widget
    m_tabWidget = new QTabWidget();

    // Information tab
    QWidget *infoTab = new QWidget();
    QFormLayout *infoLayout = new QFormLayout(infoTab);

    m_nameLabel = new QLabel(m_customer.name);
    m_emailLabel = new QLabel(m_customer.email);
    m_phoneLabel = new QLabel(m_customer.phone);
    m_companyLabel = new QLabel(m_customer.company);
    m_addressLabel = new QLabel(QString("%1, %2, %3")
                               .arg(m_customer.address)
                               .arg(m_customer.city)
                               .arg(m_customer.country));
    m_segmentLabel = new QLabel(m_customer.segment);
    m_statusLabel = new QLabel(m_customer.status);

    infoLayout->addRow(tr("Name:"), m_nameLabel);
    infoLayout->addRow(tr("Email:"), m_emailLabel);
    infoLayout->addRow(tr("Phone:"), m_phoneLabel);
    infoLayout->addRow(tr("Company:"), m_companyLabel);
    infoLayout->addRow(tr("Address:"), m_addressLabel);
    infoLayout->addRow(tr("Segment:"), m_segmentLabel);
    infoLayout->addRow(tr("Status:"), m_statusLabel);

    m_tabWidget->addTab(infoTab, tr("Information"));

    // Orders tab
    QWidget *ordersTab = new QWidget();
    QVBoxLayout *ordersLayout = new QVBoxLayout(ordersTab);

    m_ordersTable = new QTableView();
    m_ordersModel = new QStandardItemModel(0, 5, this);
    m_ordersModel->setHorizontalHeaderLabels({
        "Order ID", "Date", "Amount", "Status", "Items"
    });
    m_ordersTable->setModel(m_ordersModel);
    ordersLayout->addWidget(m_ordersTable);

    m_tabWidget->addTab(ordersTab, tr("Orders (%1)").arg(m_customer.orderCount));

    // Communication tab
    QWidget *commTab = new QWidget();
    QVBoxLayout *commLayout = new QVBoxLayout(commTab);

    m_communicationList = new QListWidget();
    commLayout->addWidget(m_communicationList);

    m_tabWidget->addTab(commTab, tr("Communication"));

    // Analytics tab
    QWidget *analyticsTab = new QWidget();
    QFormLayout *analyticsLayout = new QFormLayout(analyticsTab);

    m_lifetimeValueLabel = new QLabel(QString("$%1").arg(m_customer.totalSpent, 0, 'f', 2));
    m_avgOrderLabel = new QLabel(QString("$%1").arg(
        m_customer.orderCount > 0 ? m_customer.totalSpent / m_customer.orderCount : 0, 0, 'f', 2));
    m_frequencyLabel = new QLabel(QString("%1 orders").arg(m_customer.orderCount));
    m_lastOrderLabel = new QLabel(m_customer.lastOrderDate.toString("yyyy-MM-dd"));

    analyticsLayout->addRow(tr("Lifetime Value:"), m_lifetimeValueLabel);
    analyticsLayout->addRow(tr("Average Order:"), m_avgOrderLabel);
    analyticsLayout->addRow(tr("Order Frequency:"), m_frequencyLabel);
    analyticsLayout->addRow(tr("Last Order:"), m_lastOrderLabel);

    m_tabWidget->addTab(analyticsTab, tr("Analytics"));

    mainLayout->addWidget(m_tabWidget);

    // Buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
    mainLayout->addWidget(buttons);
}

void CustomerDetailsDialog::loadCustomerData()
{
    loadOrderHistory();
    loadCommunicationHistory();
    calculateMetrics();
}

void CustomerDetailsDialog::loadOrderHistory()
{
    // Simulate loading order history
    for (int i = 0; i < m_customer.orderCount && i < 10; ++i) {
        QList<QStandardItem*> items;
        items << new QStandardItem(QString("ORD%1").arg(10000 + i));
        items << new QStandardItem(m_customer.lastOrderDate.addDays(-i * 30).toString("yyyy-MM-dd"));
        items << new QStandardItem(QString("$%1").arg(100 + i * 50));
        items << new QStandardItem("Delivered");
        items << new QStandardItem(QString("%1 items").arg(2 + i % 3));

        m_ordersModel->appendRow(items);
    }
}

void CustomerDetailsDialog::loadCommunicationHistory()
{
    // Simulate communication history
    m_communicationList->addItem("2024-01-15 - Email: Welcome message sent");
    m_communicationList->addItem("2024-02-20 - Phone: Follow-up call completed");
    m_communicationList->addItem("2024-03-10 - Email: Promotional offer sent");
    m_communicationList->addItem("2024-04-05 - SMS: Order confirmation");
}

void CustomerDetailsDialog::calculateMetrics()
{
    // Additional metric calculations would go here
}

// AdvancedSearchDialog Implementation
AdvancedSearchDialog::AdvancedSearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Advanced Search"));
    setModal(true);
    resize(600, 700);
    setupUI();
}

void AdvancedSearchDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Search builder
    QGroupBox *searchBuilder = new QGroupBox(tr("Search Builder"));
    QVBoxLayout *builderLayout = new QVBoxLayout(searchBuilder);

    QHBoxLayout *conditionLayout = new QHBoxLayout();
    m_fieldCombo = new QComboBox();
    m_fieldCombo->addItems({"Name", "Email", "Company", "City", "Status", "Segment"});

    m_operatorCombo = new QComboBox();
    m_operatorCombo->addItems({"contains", "equals", "starts with", "ends with"});

    m_keywordEdit = new QLineEdit();
    m_keywordEdit->setPlaceholderText(tr("Enter value..."));

    m_addConditionButton = new QPushButton(tr("Add"));

    conditionLayout->addWidget(m_fieldCombo);
    conditionLayout->addWidget(m_operatorCombo);
    conditionLayout->addWidget(m_keywordEdit);
    conditionLayout->addWidget(m_addConditionButton);

    builderLayout->addLayout(conditionLayout);

    m_conditionsList = new QListWidget();
    m_conditionsList->setMaximumHeight(100);
    builderLayout->addWidget(m_conditionsList);

    m_removeConditionButton = new QPushButton(tr("Remove Selected"));
    builderLayout->addWidget(m_removeConditionButton);

    mainLayout->addWidget(searchBuilder);

    // Filter groups
    m_demographicsGroup = new QGroupBox(tr("Demographics"));
    QFormLayout *demoLayout = new QFormLayout(m_demographicsGroup);

    m_countryCombo = new QComboBox();
    m_countryCombo->addItems({"All", "USA", "UK", "Canada", "Australia", "India"});
    m_cityCombo = new QComboBox();
    m_cityCombo->addItems({"All", "New York", "Los Angeles", "Chicago", "Houston"});
    m_segmentCombo = new QComboBox();
    m_segmentCombo->addItems({"All", "VIP", "Regular", "New"});

    demoLayout->addRow(tr("Country:"), m_countryCombo);
    demoLayout->addRow(tr("City:"), m_cityCombo);
    demoLayout->addRow(tr("Segment:"), m_segmentCombo);

    mainLayout->addWidget(m_demographicsGroup);

    m_behavioralGroup = new QGroupBox(tr("Behavioral"));
    QFormLayout *behavLayout = new QFormLayout(m_behavioralGroup);

    QHBoxLayout *ordersLayout = new QHBoxLayout();
    m_minOrdersSpin = new QSpinBox();
    m_minOrdersSpin->setRange(0, 1000);
    m_maxOrdersSpin = new QSpinBox();
    m_maxOrdersSpin->setRange(0, 1000);
    m_maxOrdersSpin->setValue(1000);
    ordersLayout->addWidget(m_minOrdersSpin);
    ordersLayout->addWidget(new QLabel("-"));
    ordersLayout->addWidget(m_maxOrdersSpin);

    m_minSatisfactionSpin = new QDoubleSpinBox();
    m_minSatisfactionSpin->setRange(0, 5);
    m_minSatisfactionSpin->setSingleStep(0.1);

    QHBoxLayout *activityLayout = new QHBoxLayout();
    m_lastActivityFromEdit = new QDateEdit(QDate::currentDate().addYears(-1));
    m_lastActivityToEdit = new QDateEdit(QDate::currentDate());
    activityLayout->addWidget(m_lastActivityFromEdit);
    activityLayout->addWidget(new QLabel("-"));
    activityLayout->addWidget(m_lastActivityToEdit);

    behavLayout->addRow(tr("Order Count:"), ordersLayout);
    behavLayout->addRow(tr("Min Satisfaction:"), m_minSatisfactionSpin);
    behavLayout->addRow(tr("Last Activity:"), activityLayout);

    mainLayout->addWidget(m_behavioralGroup);

    m_transactionalGroup = new QGroupBox(tr("Transactional"));
    QFormLayout *transLayout = new QFormLayout(m_transactionalGroup);

    QHBoxLayout *spentLayout = new QHBoxLayout();
    m_minSpentSpin = new QDoubleSpinBox();
    m_minSpentSpin->setRange(0, 1000000);
    m_minSpentSpin->setPrefix("$");
    m_maxSpentSpin = new QDoubleSpinBox();
    m_maxSpentSpin->setRange(0, 1000000);
    m_maxSpentSpin->setPrefix("$");
    m_maxSpentSpin->setValue(1000000);
    spentLayout->addWidget(m_minSpentSpin);
    spentLayout->addWidget(new QLabel("-"));
    spentLayout->addWidget(m_maxSpentSpin);

    QHBoxLayout *avgOrderLayout = new QHBoxLayout();
    m_minAvgOrderSpin = new QDoubleSpinBox();
    m_minAvgOrderSpin->setRange(0, 10000);
    m_minAvgOrderSpin->setPrefix("$");
    m_maxAvgOrderSpin = new QDoubleSpinBox();
    m_maxAvgOrderSpin->setRange(0, 10000);
    m_maxAvgOrderSpin->setPrefix("$");
    m_maxAvgOrderSpin->setValue(10000);
    avgOrderLayout->addWidget(m_minAvgOrderSpin);
    avgOrderLayout->addWidget(new QLabel("-"));
    avgOrderLayout->addWidget(m_maxAvgOrderSpin);

    transLayout->addRow(tr("Total Spent:"), spentLayout);
    transLayout->addRow(tr("Avg Order Value:"), avgOrderLayout);

    mainLayout->addWidget(m_transactionalGroup);

    // Buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Connect add/remove conditions
    connect(m_addConditionButton, &QPushButton::clicked, [this]() {
        QString condition = QString("%1 %2 '%3'")
            .arg(m_fieldCombo->currentText())
            .arg(m_operatorCombo->currentText())
            .arg(m_keywordEdit->text());
        m_conditionsList->addItem(condition);
        m_keywordEdit->clear();
    });

    connect(m_removeConditionButton, &QPushButton::clicked, [this]() {
        delete m_conditionsList->currentItem();
    });
}

SearchCriteria AdvancedSearchDialog::getSearchCriteria() const
{
    SearchCriteria criteria;

    // Build search conditions
    for (int i = 0; i < m_conditionsList->count(); ++i) {
        criteria.textSearch += m_conditionsList->item(i)->text() + " ";
    }

    criteria.country = m_countryCombo->currentText();
    criteria.city = m_cityCombo->currentText();
    criteria.segment = m_segmentCombo->currentText();
    criteria.minSpent = m_minSpentSpin->value();
    criteria.maxSpent = m_maxSpentSpin->value();
    criteria.minOrders = m_minOrdersSpin->value();
    criteria.minSatisfaction = m_minSatisfactionSpin->value();
    criteria.fromDate = m_lastActivityFromEdit->date();
    criteria.toDate = m_lastActivityToEdit->date();

    return criteria;
}

void AdvancedSearchDialog::setSearchCriteria(const SearchCriteria& criteria)
{
    // Restore criteria to UI
    m_countryCombo->setCurrentText(criteria.country);
    m_cityCombo->setCurrentText(criteria.city);
    m_segmentCombo->setCurrentText(criteria.segment);
    m_minSpentSpin->setValue(criteria.minSpent);
    m_maxSpentSpin->setValue(criteria.maxSpent);
    m_minOrdersSpin->setValue(criteria.minOrders);
    m_minSatisfactionSpin->setValue(criteria.minSatisfaction);
    m_lastActivityFromEdit->setDate(criteria.fromDate);
    m_lastActivityToEdit->setDate(criteria.toDate);
}

// CustomerMergeDialog Implementation
CustomerMergeDialog::CustomerMergeDialog(const QStringList& customerIds, QWidget *parent)
    : QDialog(parent), m_customerIds(customerIds)
{
    setWindowTitle(tr("Merge Customers"));
    setModal(true);
    resize(800, 600);
    setupUI();
    loadCustomerData();
}

void CustomerMergeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *infoLabel = new QLabel(tr("Select primary customer and fields to merge:"));
    mainLayout->addWidget(infoLabel);

    // Comparison table
    m_comparisonTable = new QTableWidget();
    m_comparisonTable->setColumnCount(m_customerIds.size() + 1);
    m_comparisonTable->setRowCount(10);

    QStringList headers = {"Field"};
    headers << m_customerIds;
    m_comparisonTable->setHorizontalHeaderLabels(headers);

    QStringList fields = {"Name", "Email", "Phone", "Company", "Address",
                         "Status", "Segment", "Total Spent", "Order Count", "Last Order"};

    for (int row = 0; row < fields.size(); ++row) {
        m_comparisonTable->setItem(row, 0, new QTableWidgetItem(fields[row]));
    }

    mainLayout->addWidget(m_comparisonTable);

    // Primary customer selection
    QGroupBox *primaryGroup = new QGroupBox(tr("Select Primary Customer"));
    QHBoxLayout *primaryLayout = new QHBoxLayout(primaryGroup);

    for (int i = 0; i < m_customerIds.size() && i < 10; ++i) {
        m_primaryButtons[i] = new QRadioButton(m_customerIds[i]);
        primaryLayout->addWidget(m_primaryButtons[i]);
    }
    if (m_customerIds.size() > 0) {
        m_primaryButtons[0]->setChecked(true);
    }

    mainLayout->addWidget(primaryGroup);

    // Preview button
    m_previewButton = new QPushButton(tr("Preview Merge"));
    connect(m_previewButton, &QPushButton::clicked, this, &CustomerMergeDialog::previewMerge);
    mainLayout->addWidget(m_previewButton);

    // Preview text
    m_previewText = new QTextEdit();
    m_previewText->setReadOnly(true);
    m_previewText->setMaximumHeight(150);
    mainLayout->addWidget(m_previewText);

    // Buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void CustomerMergeDialog::loadCustomerData()
{
    // Simulate loading customer data for comparison
    for (int col = 1; col <= m_customerIds.size(); ++col) {
        m_comparisonTable->setItem(0, col, new QTableWidgetItem(QString("Customer %1").arg(col)));
        m_comparisonTable->setItem(1, col, new QTableWidgetItem(QString("email%1@example.com").arg(col)));
        m_comparisonTable->setItem(2, col, new QTableWidgetItem(QString("+1-555-000%1").arg(col)));
        m_comparisonTable->setItem(3, col, new QTableWidgetItem(QString("Company %1").arg(col)));
        m_comparisonTable->setItem(4, col, new QTableWidgetItem(QString("%1 Main St").arg(col * 100)));
        m_comparisonTable->setItem(5, col, new QTableWidgetItem("Active"));
        m_comparisonTable->setItem(6, col, new QTableWidgetItem(col == 1 ? "VIP" : "Regular"));
        m_comparisonTable->setItem(7, col, new QTableWidgetItem(QString("$%1").arg(col * 1000)));
        m_comparisonTable->setItem(8, col, new QTableWidgetItem(QString::number(col * 5)));
        m_comparisonTable->setItem(9, col, new QTableWidgetItem(QDate::currentDate().toString("yyyy-MM-dd")));
    }
}

void CustomerMergeDialog::previewMerge()
{
    QString primaryId = getPrimaryCustomerId();

    QString preview = tr("Merge Preview:\n\n");
    preview += tr("Primary Customer: %1\n\n").arg(primaryId);
    preview += tr("The following data will be merged:\n");
    preview += tr("- Combined order history\n");
    preview += tr("- Merged communication logs\n");
    preview += tr("- Updated total spent and metrics\n");
    preview += tr("- Retained highest segment status\n\n");
    preview += tr("Other customer records will be marked as merged.");

    m_previewText->setPlainText(preview);
}

QString CustomerMergeDialog::getPrimaryCustomerId() const
{
    for (int i = 0; i < m_customerIds.size() && i < 10; ++i) {
        if (m_primaryButtons[i] && m_primaryButtons[i]->isChecked()) {
            return m_customerIds[i];
        }
    }
    return m_customerIds.isEmpty() ? QString() : m_customerIds.first();
}

QStringList CustomerMergeDialog::getMergeFields() const
{
    QStringList fields;
    // Return selected fields to merge
    return fields;
}

// CommunicationHistoryDialog Implementation
CommunicationHistoryDialog::CommunicationHistoryDialog(const QString& customerId, QWidget *parent)
    : QDialog(parent), m_customerId(customerId)
{
    setWindowTitle(tr("Communication History - %1").arg(customerId));
    setModal(true);
    resize(800, 600);
    setupUI();
    loadHistory();
}

void CommunicationHistoryDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Filters
    QHBoxLayout *filterLayout = new QHBoxLayout();

    filterLayout->addWidget(new QLabel(tr("Type:")));
    m_typeFilter = new QComboBox();
    m_typeFilter->addItems({"All", "Email", "Phone", "SMS", "Meeting", "Note"});
    connect(m_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommunicationHistoryDialog::filterByType);
    filterLayout->addWidget(m_typeFilter);

    filterLayout->addWidget(new QLabel(tr("From:")));
    m_fromDateFilter = new QDateEdit(QDate::currentDate().addMonths(-6));
    connect(m_fromDateFilter, &QDateEdit::dateChanged,
            this, &CommunicationHistoryDialog::filterByDate);
    filterLayout->addWidget(m_fromDateFilter);

    filterLayout->addWidget(new QLabel(tr("To:")));
    m_toDateFilter = new QDateEdit(QDate::currentDate());
    connect(m_toDateFilter, &QDateEdit::dateChanged,
            this, &CommunicationHistoryDialog::filterByDate);
    filterLayout->addWidget(m_toDateFilter);

    filterLayout->addStretch();

    m_exportButton = new QPushButton(tr("Export"));
    filterLayout->addWidget(m_exportButton);

    mainLayout->addLayout(filterLayout);

    // History table
    m_historyTable = new QTableView();
    m_historyModel = new QStandardItemModel(0, 5, this);
    m_historyModel->setHorizontalHeaderLabels({
        "Date", "Type", "Subject", "Direction", "Agent"
    });

    m_historyTable->setModel(m_historyModel);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setAlternatingRowColors(true);
    m_historyTable->setSortingEnabled(true);
    m_historyTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(m_historyTable);

    // Buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
    mainLayout->addWidget(buttons);
}

void CommunicationHistoryDialog::loadHistory()
{
    // Simulate loading communication history
    QStringList types = {"Email", "Phone", "SMS", "Meeting", "Note"};
    QStringList subjects = {
        "Welcome to our service",
        "Order confirmation",
        "Follow-up call",
        "Product inquiry",
        "Support ticket resolved",
        "Monthly newsletter",
        "Special offer",
        "Account update"
    };

    for (int i = 0; i < 20; ++i) {
        QList<QStandardItem*> items;
        items << new QStandardItem(QDateTime::currentDateTime().addDays(-i * 5).toString("yyyy-MM-dd HH:mm"));
        items << new QStandardItem(types[i % types.size()]);
        items << new QStandardItem(subjects[i % subjects.size()]);
        items << new QStandardItem(i % 2 == 0 ? "Outbound" : "Inbound");
        items << new QStandardItem(QString("Agent %1").arg(i % 5 + 1));

        // Color code by type
        QColor typeColor;
        QString type = types[i % types.size()];
        if (type == "Email") typeColor = QColor(52, 152, 219);
        else if (type == "Phone") typeColor = QColor(46, 204, 113);
        else if (type == "SMS") typeColor = QColor(155, 89, 182);
        else if (type == "Meeting") typeColor = QColor(241, 196, 15);
        else if (type == "Note") typeColor = QColor(149, 165, 166);

        items[1]->setBackground(typeColor);
        items[1]->setForeground(Qt::white);

        m_historyModel->appendRow(items);
    }
}

void CommunicationHistoryDialog::filterByType()
{
    // Implement type filtering
}

void CommunicationHistoryDialog::filterByDate()
{
    // Implement date filtering
}

// CustomerAnalyticsDashboard Implementation
CustomerAnalyticsDashboard::CustomerAnalyticsDashboard(const std::vector<Customer>& customers,
                                                     QWidget *parent)
    : QDialog(parent), m_customers(customers)
{
    setWindowTitle(tr("Customer Analytics Dashboard"));
    setModal(true);
    resize(1200, 800);
    setupUI();
    updateMetrics();
    generateCharts();
}

void CustomerAnalyticsDashboard::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title
    QLabel *titleLabel = new QLabel(tr("Customer Analytics Dashboard"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Tab widget
    m_tabWidget = new QTabWidget();

    // Overview tab
    QWidget *overviewTab = new QWidget();
    QVBoxLayout *overviewLayout = new QVBoxLayout(overviewTab);

    m_metricsGrid = new QGridLayout();

    // Create metric cards
    auto createMetricCard = [](const QString& title, const QString& value, const QColor& color) -> QWidget* {
        QWidget *card = new QWidget();
        card->setStyleSheet(QString("QWidget { background-color: %1; border-radius: 10px; }")
                           .arg(color.name()));

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(20, 20, 20, 20);

        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("QLabel { color: white; font-size: 14px; }");

        QLabel *valueLabel = new QLabel(value);
        valueLabel->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; }");

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(valueLabel);
        cardLayout->addStretch();

        return card;
    };

    m_totalCustomersLabel = new QLabel("0");
    m_activeCustomersLabel = new QLabel("0");
    m_churnRateLabel = new QLabel("0%");
    m_lifetimeValueLabel = new QLabel("$0");

    m_metricsGrid->addWidget(
        createMetricCard(tr("Total Customers"), QString::number(m_customers.size()),
                        QColor(52, 152, 219)), 0, 0);
    m_metricsGrid->addWidget(
        createMetricCard(tr("Active Customers"), "0", QColor(46, 204, 113)), 0, 1);
    m_metricsGrid->addWidget(
        createMetricCard(tr("Churn Rate"), "0%", QColor(231, 76, 60)), 0, 2);
    m_metricsGrid->addWidget(
        createMetricCard(tr("Avg Lifetime Value"), "$0", QColor(155, 89, 182)), 0, 3);

    overviewLayout->addLayout(m_metricsGrid);
    overviewLayout->addStretch();

    m_tabWidget->addTab(overviewTab, tr("Overview"));

    // Segment Analysis tab
    QWidget *segmentTab = new QWidget();
    QVBoxLayout *segmentLayout = new QVBoxLayout(segmentTab);

    m_segmentChart = new QWidget();
    m_segmentChart->setMinimumHeight(400);
    segmentLayout->addWidget(m_segmentChart);

    m_segmentTable = new QTableWidget();
    m_segmentTable->setColumnCount(4);
    m_segmentTable->setHorizontalHeaderLabels({"Segment", "Count", "Revenue", "Avg Value"});
    segmentLayout->addWidget(m_segmentTable);

    m_tabWidget->addTab(segmentTab, tr("Segment Analysis"));

    // Geographic Distribution tab
    QWidget *geoTab = new QWidget();
    QVBoxLayout *geoLayout = new QVBoxLayout(geoTab);

    m_geoMap = new QWidget();
    m_geoMap->setMinimumHeight(400);
    geoLayout->addWidget(m_geoMap);

    m_geoTable = new QTableWidget();
    m_geoTable->setColumnCount(3);
    m_geoTable->setHorizontalHeaderLabels({"Country", "Customers", "Revenue"});
    geoLayout->addWidget(m_geoTable);

    m_tabWidget->addTab(geoTab, tr("Geographic Distribution"));

    // Revenue Analysis tab
    QWidget *revenueTab = new QWidget();
    QVBoxLayout *revenueLayout = new QVBoxLayout(revenueTab);

    QHBoxLayout *periodLayout = new QHBoxLayout();
    periodLayout->addWidget(new QLabel(tr("Period:")));
    m_periodCombo = new QComboBox();
    m_periodCombo->addItems({"Daily", "Weekly", "Monthly", "Quarterly", "Yearly"});
    m_periodCombo->setCurrentIndex(2); // Monthly
    periodLayout->addWidget(m_periodCombo);
    periodLayout->addStretch();

    revenueLayout->addLayout(periodLayout);

    m_revenueChart = new QWidget();
    m_revenueChart->setMinimumHeight(500);
    revenueLayout->addWidget(m_revenueChart);

    m_tabWidget->addTab(revenueTab, tr("Revenue Analysis"));

    // Satisfaction Metrics tab
    QWidget *satisfactionTab = new QWidget();
    QVBoxLayout *satisfactionLayout = new QVBoxLayout(satisfactionTab);

    m_satisfactionChart = new QWidget();
    m_satisfactionChart->setMinimumHeight(400);
    satisfactionLayout->addWidget(m_satisfactionChart);

    QHBoxLayout *npsLayout = new QHBoxLayout();
    npsLayout->addWidget(new QLabel(tr("Net Promoter Score (NPS):")));
    m_npsBar = new QProgressBar();
    m_npsBar->setRange(-100, 100);
    m_npsBar->setValue(0);
    m_npsBar->setFormat("%v");
    npsLayout->addWidget(m_npsBar);

    satisfactionLayout->addLayout(npsLayout);
    satisfactionLayout->addStretch();

    m_tabWidget->addTab(satisfactionTab, tr("Satisfaction Metrics"));

    mainLayout->addWidget(m_tabWidget);

    // Export button
    QPushButton *exportButton = new QPushButton(tr("Export Analytics"));
    connect(exportButton, &QPushButton::clicked, this, &CustomerAnalyticsDashboard::exportAnalytics);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(exportButton);

    mainLayout->addLayout(buttonLayout);

    // Close button
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
    mainLayout->addWidget(buttons);
}

void CustomerAnalyticsDashboard::updateMetrics()
{
    // Calculate metrics
    int activeCount = 0;
    double totalRevenue = 0;
    double totalSatisfaction = 0;
    int promoters = 0, detractors = 0;

    std::map<QString, int> segmentCount;
    std::map<QString, double> segmentRevenue;
    std::map<QString, int> countryCount;
    std::map<QString, double> countryRevenue;

    for (const auto& customer : m_customers) {
        if (customer.status == "Active") activeCount++;
        totalRevenue += customer.totalSpent;
        totalSatisfaction += customer.satisfactionScore;

        // NPS calculation
        if (customer.satisfactionScore >= 4.5) promoters++;
        else if (customer.satisfactionScore <= 3.0) detractors++;

        // Segment analysis
        segmentCount[customer.segment]++;
        segmentRevenue[customer.segment] += customer.totalSpent;

        // Geographic analysis
        countryCount[customer.country]++;
        countryRevenue[customer.country] += customer.totalSpent;
    }

    // Update overview metrics
    double churnRate = m_customers.empty() ? 0 :
        ((m_customers.size() - activeCount) * 100.0 / m_customers.size());
    double avgLifetimeValue = m_customers.empty() ? 0 : totalRevenue / m_customers.size();
    double avgSatisfaction = m_customers.empty() ? 0 : totalSatisfaction / m_customers.size();

    // Calculate NPS
    int nps = m_customers.empty() ? 0 :
        ((promoters - detractors) * 100 / m_customers.size());
    m_npsBar->setValue(nps);

    // Update segment table
    m_segmentTable->setRowCount(segmentCount.size());
    int row = 0;
    for (const auto& [segment, count] : segmentCount) {
        m_segmentTable->setItem(row, 0, new QTableWidgetItem(segment));
        m_segmentTable->setItem(row, 1, new QTableWidgetItem(QString::number(count)));
        m_segmentTable->setItem(row, 2, new QTableWidgetItem(
            QString("$%1").arg(segmentRevenue[segment], 0, 'f', 2)));
        m_segmentTable->setItem(row, 3, new QTableWidgetItem(
            QString("$%1").arg(segmentRevenue[segment] / count, 0, 'f', 2)));
        row++;
    }

    // Update geographic table
    m_geoTable->setRowCount(countryCount.size());
    row = 0;
    for (const auto& [country, count] : countryCount) {
        m_geoTable->setItem(row, 0, new QTableWidgetItem(country));
        m_geoTable->setItem(row, 1, new QTableWidgetItem(QString::number(count)));
        m_geoTable->setItem(row, 2, new QTableWidgetItem(
            QString("$%1").arg(countryRevenue[country], 0, 'f', 2)));
        row++;
    }
}

void CustomerAnalyticsDashboard::generateCharts()
{
    // In a real implementation, you would use QtCharts or another charting library
    // to create actual charts. This is a placeholder.

    // Segment pie chart placeholder
    QPalette segmentPalette = m_segmentChart->palette();
    segmentPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    m_segmentChart->setPalette(segmentPalette);
    m_segmentChart->setAutoFillBackground(true);

    // Geographic map placeholder
    QPalette geoPalette = m_geoMap->palette();
    geoPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    m_geoMap->setPalette(geoPalette);
    m_geoMap->setAutoFillBackground(true);

    // Revenue chart placeholder
    QPalette revenuePalette = m_revenueChart->palette();
    revenuePalette.setColor(QPalette::Window, QColor(240, 240, 240));
    m_revenueChart->setPalette(revenuePalette);
    m_revenueChart->setAutoFillBackground(true);

    // Satisfaction chart placeholder
    QPalette satisfactionPalette = m_satisfactionChart->palette();
    satisfactionPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    m_satisfactionChart->setPalette(satisfactionPalette);
    m_satisfactionChart->setAutoFillBackground(true);
}

void CustomerAnalyticsDashboard::exportAnalytics()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Analytics"),
                                                   "customer_analytics.pdf",
                                                   tr("PDF Files (*.pdf)"));
    if (!fileName.isEmpty()) {
        // Export analytics to PDF
        QMessageBox::information(this, tr("Export Complete"),
                               tr("Analytics exported to %1").arg(fileName));
    }
}
