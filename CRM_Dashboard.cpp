#include "CRM_Dashboard.h"

CRM_Dashboard::CRM_Dashboard(QWidget *parent, const QString &userEmail, const QString &userName)
    : QMainWindow(parent), currentUserEmail(userEmail), currentUserName(userName)
{
    setWindowTitle("CRM System - Dashboard");
    setMinimumSize(1024, 768);

    setupUI();
    createMenu();
    createToolBar();
    createStatusBar();
    populateSampleData();
}

CRM_Dashboard::~CRM_Dashboard()
{
}

void CRM_Dashboard::setupUI()
{
    // Main Tab Widget
    mainTabs = new QTabWidget(this);
    setCentralWidget(mainTabs);

    // Customer Management Tab
    QWidget *customerTab = new QWidget();
    QVBoxLayout *customerLayout = new QVBoxLayout(customerTab);

    // Search Bar
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchField = new QLineEdit();
    searchFilter = new QComboBox();
    searchFilter->addItems({"Name", "Email", "Phone", "Company", "All"});
    searchButton = new QPushButton("Search");

    searchLayout->addWidget(searchFilter);
    searchLayout->addWidget(searchField);
    searchLayout->addWidget(searchButton);

    // Action Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add Customer");
    editButton = new QPushButton("Edit Customer");
    deleteButton = new QPushButton("Delete Customer");
    refreshButton = new QPushButton("Refresh");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(refreshButton);

    // Customer Table
    customerTable = new QTableWidget();
    customerTable->setColumnCount(6);
    customerTable->setHorizontalHeaderLabels({"ID", "Name", "Email", "Phone", "Company", "Last Contact"});
    customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Assemble Customer Tab
    customerLayout->addLayout(searchLayout);
    customerLayout->addLayout(buttonLayout);
    customerLayout->addWidget(customerTable);

    // Add tabs
    mainTabs->addTab(customerTab, "Customer Management");

    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &CRM_Dashboard::addCustomer);
    connect(editButton, &QPushButton::clicked, this, &CRM_Dashboard::updateCustomer);
    connect(deleteButton, &QPushButton::clicked, this, &CRM_Dashboard::deleteCustomer);
    connect(searchButton, &QPushButton::clicked, this, &CRM_Dashboard::searchCustomer);
    connect(refreshButton, &QPushButton::clicked, this, &CRM_Dashboard::refreshCustomerList);
}

void CRM_Dashboard::populateSampleData()
{
    // Clear existing data
    customerTable->setRowCount(0);

    // Add sample customers
    QStringList customers[5] = {
        {"1", "John Doe", "john@example.com", "555-0101", "Acme Corp", "2023-05-15"},
        {"2", "Jane Smith", "jane@example.com", "555-0102", "Globex", "2023-06-20"},
        {"3", "Bob Johnson", "bob@example.com", "555-0103", "Initech", "2023-04-10"},
        {"4", "Alice Brown", "alice@example.com", "555-0104", "Umbrella Corp", "2023-07-05"},
        {"5", "Charlie Wilson", "charlie@example.com", "555-0105", "Wayne Ent", "2023-03-28"}
    };

    for (int i = 0; i < 5; i++) {
        int row = customerTable->rowCount();
        customerTable->insertRow(row);

        for (int col = 0; col < 6; col++) {
            QTableWidgetItem *item = new QTableWidgetItem(customers[i][col]);
            customerTable->setItem(row, col, item);
        }
    }
}

void CRM_Dashboard::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    actExport = fileMenu->addAction("&Export Data");
    actPrint = fileMenu->addAction("&Print");
    fileMenu->addSeparator();
    actExit = fileMenu->addAction("E&xit");

    QMenu *customerMenu = menuBar()->addMenu("&Customers");
    actAddCustomer = customerMenu->addAction("&Add Customer");
    actSearchCustomer = customerMenu->addAction("&Search Customer");
    actRefresh = customerMenu->addAction("&Refresh");

    QMenu *reportsMenu = menuBar()->addMenu("&Reports");
    actAnalytics = reportsMenu->addAction("&Analytics");

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    actAbout = helpMenu->addAction("&About");

    // Connect menu actions
    connect(actAddCustomer, &QAction::triggered, this, &CRM_Dashboard::addCustomer);
    connect(actSearchCustomer, &QAction::triggered, this, &CRM_Dashboard::searchCustomer);
    connect(actRefresh, &QAction::triggered, this, &CRM_Dashboard::refreshCustomerList);
    connect(actAnalytics, &QAction::triggered, this, &CRM_Dashboard::showAnalytics);
    connect(actExport, &QAction::triggered, this, &CRM_Dashboard::exportData);
    connect(actPrint, &QAction::triggered, this, &CRM_Dashboard::printData);
    connect(actAbout, &QAction::triggered, this, &CRM_Dashboard::about);
    connect(actExit, &QAction::triggered, this, &QMainWindow::close);
}

void CRM_Dashboard::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");
    toolBar->addAction(actAddCustomer);
    toolBar->addAction(actSearchCustomer);
    toolBar->addAction(actRefresh);
    toolBar->addSeparator();
    toolBar->addAction(actAnalytics);
}

void CRM_Dashboard::createStatusBar()
{
    statusBar()->showMessage(QString("Logged in as: %1 (%2) | %3 customers")
                                 .arg(currentUserName)
                                 .arg(currentUserEmail)
                                 .arg(customerTable->rowCount()));
}

void CRM_Dashboard::addCustomer()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Add Customer", "Customer Name:", QLineEdit::Normal, "", &ok);

    if (ok && !name.isEmpty()) {
        int newId = customerTable->rowCount() + 1;
        int row = customerTable->rowCount();
        customerTable->insertRow(row);

        customerTable->setItem(row, 0, new QTableWidgetItem(QString::number(newId)));
        customerTable->setItem(row, 1, new QTableWidgetItem(name));
        customerTable->setItem(row, 2, new QTableWidgetItem(""));
        customerTable->setItem(row, 3, new QTableWidgetItem(""));
        customerTable->setItem(row, 4, new QTableWidgetItem(""));
        customerTable->setItem(row, 5, new QTableWidgetItem(QDate::currentDate().toString("yyyy-MM-dd")));

        statusBar()->showMessage("Customer added successfully", 3000);
    }
}

void CRM_Dashboard::searchCustomer()
{
    QString searchText = searchField->text().toLower();
    QString filter = searchFilter->currentText();

    if (searchText.isEmpty()) {
        // Show all rows if search is empty
        for (int i = 0; i < customerTable->rowCount(); ++i) {
            customerTable->showRow(i);
        }
        statusBar()->showMessage("Showing all customers", 3000);
        return;
    }

    int visibleCount = 0;

    for (int i = 0; i < customerTable->rowCount(); ++i) {
        bool matchFound = false;

        if (filter == "All") {
            // Search all columns
            for (int col = 1; col < customerTable->columnCount(); ++col) {
                if (customerTable->item(i, col)->text().toLower().contains(searchText)) {
                    matchFound = true;
                    break;
                }
            }
        } else {
            // Search specific column
            int col = -1;
            if (filter == "Name") col = 1;
            else if (filter == "Email") col = 2;
            else if (filter == "Phone") col = 3;
            else if (filter == "Company") col = 4;

            if (col != -1 && customerTable->item(i, col)->text().toLower().contains(searchText)) {
                matchFound = true;
            }
        }

        customerTable->setRowHidden(i, !matchFound);
        if (matchFound) visibleCount++;
    }

    statusBar()->showMessage(QString("Found %1 customers").arg(visibleCount), 3000);
}

void CRM_Dashboard::updateCustomer()
{
    QList<QTableWidgetItem *> selected = customerTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a customer to edit");
        return;
    }

    int row = selected.first()->row();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Customer", "New Name:",
                                            QLineEdit::Normal, customerTable->item(row, 1)->text(), &ok);

    if (ok && !newName.isEmpty()) {
        customerTable->item(row, 1)->setText(newName);
        statusBar()->showMessage("Customer updated successfully", 3000);
    }
}

void CRM_Dashboard::deleteCustomer()
{
    QList<QTableWidgetItem *> selected = customerTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a customer to delete");
        return;
    }

    int row = selected.first()->row();
    QString customerName = customerTable->item(row, 1)->text();

    if (QMessageBox::question(this, "Confirm Delete",
                              QString("Are you sure you want to delete customer '%1'?").arg(customerName),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        customerTable->removeRow(row);
        statusBar()->showMessage("Customer deleted successfully", 3000);
    }
}

void CRM_Dashboard::refreshCustomerList()
{
    for (int i = 0; i < customerTable->rowCount(); ++i) {
        customerTable->showRow(i);
    }
    statusBar()->showMessage("Customer list refreshed", 2000);
}

void CRM_Dashboard::showAnalytics()
{
    QMessageBox::information(this, "Analytics",
                             QString("Customer Count: %1\nLogged in as: %2")
                                 .arg(customerTable->rowCount())
                                 .arg(currentUserName));
}

void CRM_Dashboard::exportData()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Data", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing");
        return;
    }

    QTextStream out(&file);

    // Write headers
    for (int col = 0; col < customerTable->columnCount(); ++col) {
        out << customerTable->horizontalHeaderItem(col)->text();
        if (col < customerTable->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n";

    // Write data
    for (int row = 0; row < customerTable->rowCount(); ++row) {
        for (int col = 0; col < customerTable->columnCount(); ++col) {
            out << customerTable->item(row, col)->text();
            if (col < customerTable->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
    }

    file.close();
    statusBar()->showMessage("Data exported successfully", 3000);
}

void CRM_Dashboard::printData()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter;
        if (!painter.begin(&printer)) {
            QMessageBox::critical(this, "Error", "Could not open printer");
            return;
        }

        // Print table
        int rowHeight = 30;
        int colWidth = 150;

        // Print headers
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        for (int col = 0; col < customerTable->columnCount(); ++col) {
            painter.drawText(col * colWidth, rowHeight,
                             customerTable->horizontalHeaderItem(col)->text());
        }

        // Print data
        painter.setFont(QFont("Arial", 9));
        for (int row = 0; row < customerTable->rowCount(); ++row) {
            for (int col = 0; col < customerTable->columnCount(); ++col) {
                painter.drawText(col * colWidth, (row + 2) * rowHeight,
                                 customerTable->item(row, col)->text());
            }
        }

        painter.end();
        statusBar()->showMessage("Data printed successfully", 3000);
    }
}

void CRM_Dashboard::about()
{
    QMessageBox::about(this, "About CRM System",
                       QString("<h2>CRM System</h2>"
                               "<p>Version 1.0</p>"
                               "<p>Pure GUI Implementation</p>"
                               "<p>Current user: %1 (%2)</p>"
                               "<p>© 2023 Your Company</p>")
                           .arg(currentUserName).arg(currentUserEmail));
}
