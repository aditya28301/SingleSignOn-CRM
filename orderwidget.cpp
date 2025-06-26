#include "orderwidget.h"
#include "ordermanager.h"
#include "order.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QLineEdit>
#include <QInputDialog>
#include <QLabel>

OrderWidget::OrderWidget(OrderManager* orderManager, QWidget *parent)
    : QWidget(parent), m_orderManager(orderManager)
{
    setupUi();
    loadOrders();
    updateStatistics();

    connect(m_orderManager, &OrderManager::orderAdded, this, [this]() {
        refreshOrderList();
        updateStatistics();
    });
    connect(m_orderManager, &OrderManager::orderUpdated, this, &OrderWidget::refreshOrderList);
    connect(m_orderManager, &OrderManager::orderDeleted, this, [this]() {
        refreshOrderList();
        updateStatistics();
    });
}

void OrderWidget::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Statistics section
    QGroupBox* statsGroup = new QGroupBox(tr("Statistics"));
    QHBoxLayout* statsLayout = new QHBoxLayout(statsGroup);

    m_totalRevenueLabel = new QLabel(tr("Total Revenue: $0.00"));
    m_orderCountLabel = new QLabel(tr("Orders: 0"));
    m_avgOrderLabel = new QLabel(tr("Avg Order: $0.00"));

    statsLayout->addWidget(m_totalRevenueLabel);
    statsLayout->addWidget(m_orderCountLabel);
    statsLayout->addWidget(m_avgOrderLabel);
    statsLayout->addStretch();

    // Controls section
    QGroupBox* controlsGroup = new QGroupBox(tr("Controls"));
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsGroup);

    m_customerFilter = new QComboBox();
    m_customerFilter->addItem(tr("All Customers"), "");

    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({tr("Pending"), tr("Processing"), tr("Shipped"), tr("Delivered"), tr("Cancelled")});

    m_createBtn = new QPushButton(tr("Create Order"));
    m_updateBtn = new QPushButton(tr("Update Status"));
    m_deleteBtn = new QPushButton(tr("Delete Order"));
    m_viewBtn = new QPushButton(tr("View Details"));

    m_updateBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
    m_viewBtn->setEnabled(false);

    controlsLayout->addWidget(new QLabel(tr("Filter by Customer:")));
    controlsLayout->addWidget(m_customerFilter);
    controlsLayout->addWidget(new QLabel(tr("Status:")));
    controlsLayout->addWidget(m_statusCombo);
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_createBtn);
    controlsLayout->addWidget(m_updateBtn);
    controlsLayout->addWidget(m_deleteBtn);
    controlsLayout->addWidget(m_viewBtn);

    // Orders table
    m_orderTable = new QTableWidget();
    m_orderTable->setColumnCount(6);
    m_orderTable->setHorizontalHeaderLabels({
        tr("Order ID"), tr("Customer"), tr("Date"), tr("Items"), tr("Total"), tr("Status")
    });
    m_orderTable->horizontalHeader()->setStretchLastSection(true);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Add widgets to main layout
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(controlsGroup);
    mainLayout->addWidget(m_orderTable);

    // Connect signals
    connect(m_createBtn, &QPushButton::clicked, this, &OrderWidget::createOrder);
    connect(m_updateBtn, &QPushButton::clicked, this, &OrderWidget::updateOrderStatus);
    connect(m_deleteBtn, &QPushButton::clicked, this, &OrderWidget::deleteOrder);
    connect(m_viewBtn, &QPushButton::clicked, this, &OrderWidget::viewOrderDetails);
    connect(m_customerFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OrderWidget::refreshOrderList);
    connect(m_orderTable, &QTableWidget::itemSelectionChanged, this, &OrderWidget::onOrderSelectionChanged);
}

void OrderWidget::loadOrders() {
    m_orderTable->setRowCount(0);

    QList<QSharedPointer<Order>> orders;
    if (m_customerFilter->currentIndex() == 0) {
        orders = m_orderManager->getAllOrders();
    } else {
        QString customerId = m_customerFilter->currentData().toString();
        orders = m_orderManager->getOrdersByCustomer(customerId);
    }

    for (const auto& order : orders) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);

        m_orderTable->setItem(row, 0, new QTableWidgetItem(order->id()));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(order->customerName()));
        m_orderTable->setItem(row, 2, new QTableWidgetItem(order->orderDate().toString("yyyy-MM-dd hh:mm")));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(QString::number(order->items().size())));
        m_orderTable->setItem(row, 4, new QTableWidgetItem(QString("$%1").arg(order->total(), 0, 'f', 2)));

        QTableWidgetItem* statusItem = new QTableWidgetItem(Order::statusToString(order->status()));
        switch (order->status()) {
        case Order::Pending:
            statusItem->setBackground(QColor(255, 193, 7)); break;
        case Order::Processing:
            statusItem->setBackground(QColor(33, 150, 243)); break;
        case Order::Shipped:
            statusItem->setBackground(QColor(156, 39, 176)); break;
        case Order::Delivered:
            statusItem->setBackground(QColor(76, 175, 80)); break;
        case Order::Cancelled:
            statusItem->setBackground(QColor(244, 67, 54)); break;
        }
        statusItem->setForeground(Qt::white);
        m_orderTable->setItem(row, 5, statusItem);
    }
}

void OrderWidget::createOrder() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Create Order"));
    dialog.setModal(true);
    dialog.resize(600, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QHBoxLayout* customerLayout = new QHBoxLayout();
    QLabel* customerLabel = new QLabel(tr("Customer:"));
    QLineEdit* customerIdEdit = new QLineEdit();
    customerIdEdit->setPlaceholderText("Customer ID (e.g., CUST1001)");
    QLineEdit* customerNameEdit = new QLineEdit();
    customerNameEdit->setPlaceholderText("Customer Name");

    customerLayout->addWidget(customerLabel);
    customerLayout->addWidget(customerIdEdit);
    customerLayout->addWidget(customerNameEdit);

    QLabel* itemsLabel = new QLabel(tr("Order Items:"));
    QListWidget* itemsList = new QListWidget();

    QGroupBox* addItemGroup = new QGroupBox(tr("Add Item"));
    QHBoxLayout* addItemLayout = new QHBoxLayout(addItemGroup);

    QLineEdit* productEdit = new QLineEdit();
    productEdit->setPlaceholderText("Product name");
    QSpinBox* quantitySpin = new QSpinBox();
    quantitySpin->setMinimum(1);
    quantitySpin->setMaximum(999);
    quantitySpin->setValue(1);
    QDoubleSpinBox* priceSpin = new QDoubleSpinBox();
    priceSpin->setMinimum(0.01);
    priceSpin->setMaximum(9999.99);
    priceSpin->setPrefix("$");
    priceSpin->setValue(0.01);
    QPushButton* addItemBtn = new QPushButton(tr("Add Item"));

    addItemLayout->addWidget(productEdit);
    addItemLayout->addWidget(new QLabel("Qty:"));
    addItemLayout->addWidget(quantitySpin);
    addItemLayout->addWidget(new QLabel("Price:"));
    addItemLayout->addWidget(priceSpin);
    addItemLayout->addWidget(addItemBtn);

    QLabel* totalLabel = new QLabel(tr("Total: $0.00"));
    QFont totalFont;
    totalFont.setPointSize(12);
    totalFont.setBold(true);
    totalLabel->setFont(totalFont);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    mainLayout->addLayout(customerLayout);
    mainLayout->addWidget(itemsLabel);
    mainLayout->addWidget(itemsList);
    mainLayout->addWidget(addItemGroup);
    mainLayout->addWidget(totalLabel);
    mainLayout->addWidget(buttons);

    QList<OrderItem> orderItems;
    double total = 0;

    auto updateTotal = [&]() {
        total = 0;
        for (const auto& item : orderItems) total += item.total();
        totalLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
    };

    connect(addItemBtn, &QPushButton::clicked, [&]() {
        if (!productEdit->text().isEmpty()) {
            OrderItem item{productEdit->text(), quantitySpin->value(), priceSpin->value()};
            orderItems.append(item);
            itemsList->addItem(QString("%1 x%2 @ $%3 = $%4")
                                   .arg(item.productName).arg(item.quantity).arg(item.price, 0, 'f', 2).arg(item.total(), 0, 'f', 2));
            productEdit->clear(); quantitySpin->setValue(1); priceSpin->setValue(0.01);
            productEdit->setFocus();
            updateTotal();
        }
    });

    connect(productEdit, &QLineEdit::returnPressed, addItemBtn, &QPushButton::click);
    connect(priceSpin, &QDoubleSpinBox::editingFinished, addItemBtn, &QPushButton::click);

    if (dialog.exec() == QDialog::Accepted) {
        if (customerIdEdit->text().isEmpty() || customerNameEdit->text().isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Please enter customer ID and name!"));
            return;
        }
        if (orderItems.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Please add at least one item to the order!"));
            return;
        }

        auto order = QSharedPointer<Order>::create(customerIdEdit->text());
        order->setCustomerName(customerNameEdit->text());
        for (const auto& item : orderItems) order->addItem(item);
        m_orderManager->addOrder(order);

        QMessageBox::information(this, tr("Success"),
                                 tr("Order %1 created successfully! Total: $%2")
                                     .arg(order->id())
                                     .arg(order->total(), 0, 'f', 2));
    }
}

void OrderWidget::updateOrderStatus() {
    QTableWidgetItem* selectedItem = m_orderTable->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an order to update."));
        return;
    }

    int row = selectedItem->row();
    QString orderId = m_orderTable->item(row, 0)->text();

    // Get all orders and find the one we need
    QList<QSharedPointer<Order>> allOrders = m_orderManager->getAllOrders();
    QSharedPointer<Order> order;

    for (const auto& o : allOrders) {
        if (o->id() == orderId) {
            order = o;
            break;
        }
    }

    if (!order) {
        QMessageBox::warning(this, tr("Warning"), tr("Order not found."));
        return;
    }

    bool ok;
    QStringList statusOptions = {tr("Pending"), tr("Processing"), tr("Shipped"), tr("Delivered"), tr("Cancelled")};
    QString selectedStatus = QInputDialog::getItem(this, tr("Update Order Status"),
                                                   tr("Select new status:"), statusOptions,
                                                   static_cast<int>(order->status()), false, &ok);

    if (ok) {
        Order::Status newStatus = static_cast<Order::Status>(statusOptions.indexOf(selectedStatus));
        order->setStatus(newStatus);

        // Trigger refresh since we don't have updateOrder method
        emit m_orderManager->orderUpdated(order);

        QMessageBox::information(this, tr("Success"),
                                 tr("Order status updated to: %1").arg(selectedStatus));
    }
}

void OrderWidget::deleteOrder() {
    QTableWidgetItem* selectedItem = m_orderTable->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an order to delete."));
        return;
    }

    int row = selectedItem->row();
    QString orderId = m_orderTable->item(row, 0)->text();
    QString customerName = m_orderTable->item(row, 1)->text();

    int ret = QMessageBox::question(this, tr("Confirm Delete"),
                                    tr("Are you sure you want to delete order %1 for %2?")
                                        .arg(orderId, customerName),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        m_orderManager->deleteOrder(orderId);
        QMessageBox::information(this, tr("Success"), tr("Order deleted successfully."));
    }
}

void OrderWidget::refreshOrderList() {
    loadOrders();
}

void OrderWidget::onOrderSelectionChanged() {
    bool hasSelection = m_orderTable->currentRow() >= 0;
    m_updateBtn->setEnabled(hasSelection);
    m_deleteBtn->setEnabled(hasSelection);
    m_viewBtn->setEnabled(hasSelection);
}

void OrderWidget::viewOrderDetails() {
    QTableWidgetItem* selectedItem = m_orderTable->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an order to view."));
        return;
    }

    int row = selectedItem->row();
    QString orderId = m_orderTable->item(row, 0)->text();

    // Get all orders and find the one we need
    QList<QSharedPointer<Order>> allOrders = m_orderManager->getAllOrders();
    QSharedPointer<Order> order;

    for (const auto& o : allOrders) {
        if (o->id() == orderId) {
            order = o;
            break;
        }
    }

    if (!order) {
        QMessageBox::warning(this, tr("Warning"), tr("Order not found."));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Order Details - %1").arg(orderId));
    dialog.setModal(true);
    dialog.resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // Order info
    QGroupBox* infoGroup = new QGroupBox(tr("Order Information"));
    QFormLayout* infoLayout = new QFormLayout(infoGroup);

    infoLayout->addRow(tr("Order ID:"), new QLabel(order->id()));
    infoLayout->addRow(tr("Customer:"), new QLabel(order->customerName()));
    infoLayout->addRow(tr("Date:"), new QLabel(order->orderDate().toString("yyyy-MM-dd hh:mm:ss")));
    infoLayout->addRow(tr("Status:"), new QLabel(Order::statusToString(order->status())));
    infoLayout->addRow(tr("Total:"), new QLabel(QString("$%1").arg(order->total(), 0, 'f', 2)));

    // Items list
    QGroupBox* itemsGroup = new QGroupBox(tr("Order Items"));
    QVBoxLayout* itemsLayout = new QVBoxLayout(itemsGroup);

    QTableWidget* itemsTable = new QTableWidget();
    itemsTable->setColumnCount(4);
    itemsTable->setHorizontalHeaderLabels({tr("Product"), tr("Quantity"), tr("Price"), tr("Total")});
    itemsTable->horizontalHeader()->setStretchLastSection(true);

    const auto& items = order->items();
    itemsTable->setRowCount(items.size());

    for (int i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        itemsTable->setItem(i, 0, new QTableWidgetItem(item.productName));
        itemsTable->setItem(i, 1, new QTableWidgetItem(QString::number(item.quantity)));
        itemsTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(item.price, 0, 'f', 2)));
        itemsTable->setItem(i, 3, new QTableWidgetItem(QString("$%1").arg(item.total(), 0, 'f', 2)));
    }

    itemsLayout->addWidget(itemsTable);

    // Close button
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(infoGroup);
    layout->addWidget(itemsGroup);
    layout->addWidget(buttons);

    dialog.exec();
}

void OrderWidget::updateStatistics() {
    QList<QSharedPointer<Order>> allOrders = m_orderManager->getAllOrders();

    int orderCount = allOrders.size();
    double totalRevenue = 0.0;

    for (const auto& order : allOrders) {
        totalRevenue += order->total();
    }

    double avgOrder = orderCount > 0 ? totalRevenue / orderCount : 0.0;

    m_totalRevenueLabel->setText(tr("Total Revenue: $%1").arg(totalRevenue, 0, 'f', 2));
    m_orderCountLabel->setText(tr("Orders: %1").arg(orderCount));
    m_avgOrderLabel->setText(tr("Avg Order: $%1").arg(avgOrder, 0, 'f', 2));
}
