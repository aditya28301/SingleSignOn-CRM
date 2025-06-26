#include "ordermanager.h"
#include <algorithm>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

OrderManager::OrderManager(QObject *parent) : QObject(parent) {
    loadSampleOrders();
}

OrderManager::~OrderManager() {
    // Nothing to manually delete because we use QSharedPointer
}

void OrderManager::loadSampleOrders() {
    auto order1 = QSharedPointer<Order>::create("CUST1001");
    order1->setCustomerName("Aditya Darekar");
    order1->addItem({"Dell Laptop XPS 15", 1, 1299.99});
    order1->addItem({"Logitech MX Master Mouse", 2, 79.99});
    order1->addItem({"USB-C Hub", 1, 49.99});
    order1->setStatus(Order::Delivered);
    addOrder(order1);

    auto order2 = QSharedPointer<Order>::create("CUST1002");
    order2->setCustomerName("Somesh Atole");
    order2->addItem({"Samsung 27 Monitor", 2, 299.99});
    order2->addItem({"Mechanical Keyboard", 1, 149.99});
    order2->setStatus(Order::Shipped);
    addOrder(order2);

    auto order3 = QSharedPointer<Order>::create("CUST1003");
    order3->setCustomerName("Saurabh Dharmadhikari");
    order3->addItem({"Sony WH-1000XM4 Headphones", 1, 349.99});
    order3->addItem({"Anker Power Bank", 1, 59.99});
    order3->setStatus(Order::Processing);
    addOrder(order3);

    auto order4 = QSharedPointer<Order>::create("CUST1004");
    order4->setCustomerName("Madan Chitle");
    order4->addItem({"iPad Pro 11", 1, 799.99});
    order4->addItem({"Apple Pencil", 1, 129.99});
    order4->addItem({"iPad Case", 1, 79.99});
    order4->setStatus(Order::Pending);
    addOrder(order4);

    auto order5 = QSharedPointer<Order>::create("CUST1001");
    order5->setCustomerName("Aditya Darekar");
    order5->addItem({"Microsoft Office 365", 1, 99.99});
    order5->setStatus(Order::Delivered);
    addOrder(order5);
}

void OrderManager::addOrder(QSharedPointer<Order> order) {
    m_orders.append(order);
    emit orderAdded(order);
    emit dataChanged();
}

void OrderManager::updateOrderStatus(const QString& id, Order::Status status) {
    int index = findOrderIndex(id);
    if (index >= 0) {
        m_orders[index]->setStatus(status);
        emit orderUpdated(m_orders[index]);
        emit dataChanged();
    }
}

void OrderManager::deleteOrder(const QString& id) {
    int index = findOrderIndex(id);
    if (index >= 0) {
        m_orders.removeAt(index);
        emit orderDeleted(id);
        emit dataChanged();
    }
}

int OrderManager::findOrderIndex(const QString& id) const {
    for (int i = 0; i < m_orders.size(); ++i) {
        if (m_orders[i]->id() == id) {
            return i;
        }
    }
    return -1;
}

QList<QSharedPointer<Order>> OrderManager::getAllOrders() const {
    return m_orders;
}

QList<QSharedPointer<Order>> OrderManager::getOrdersByCustomer(const QString& customerId) const {
    QList<QSharedPointer<Order>> results;
    for (const auto& order : m_orders) {
        if (order->customerId() == customerId) {
            results.append(order);
        }
    }
    return results;
}

QList<QSharedPointer<Order>> OrderManager::getOrdersByStatus(Order::Status status) const {
    QList<QSharedPointer<Order>> results;
    for (const auto& order : m_orders) {
        if (order->status() == status) {
            results.append(order);
        }
    }
    return results;
}

QList<QSharedPointer<Order>> OrderManager::getOrdersByDateRange(const QDateTime& from, const QDateTime& to) const {
    QList<QSharedPointer<Order>> results;
    for (const auto& order : m_orders) {
        if (order->orderDate() >= from && order->orderDate() <= to) {
            results.append(order);
        }
    }
    return results;
}

QSharedPointer<Order> OrderManager::getOrder(const QString& id) {
    int index = findOrderIndex(id);
    return (index >= 0) ? m_orders[index] : nullptr;
}

QList<QSharedPointer<Order>> OrderManager::searchOrders(const QString& searchText) const {
    QList<QSharedPointer<Order>> results;
    QString lowerSearch = searchText.toLower();

    for (const auto& order : m_orders) {
        if (order->id().toLower().contains(lowerSearch) ||
            order->customerName().toLower().contains(lowerSearch)) {
            results.append(order);
            continue;
        }

        bool found = false;
        for (const auto& item : order->items()) {
            if (item.productName.toLower().contains(lowerSearch)) {
                results.append(order);
                found = true;
                break;
            }
        }

        if (!found && Order::statusToString(order->status()).toLower().contains(lowerSearch)) {
            results.append(order);
        }
    }

    return results;
}

double OrderManager::getTotalRevenue() const {
    double total = 0;
    for (const auto& order : m_orders) {
        total += order->total();
    }
    return total;
}

double OrderManager::getRevenueByCustomer(const QString& customerId) const {
    double total = 0;
    for (const auto& order : m_orders) {
        if (order->customerId() == customerId) {
            total += order->total();
        }
    }
    return total;
}

double OrderManager::getRevenueByDateRange(const QDateTime& from, const QDateTime& to) const {
    double total = 0;
    for (const auto& order : m_orders) {
        if (order->orderDate() >= from && order->orderDate() <= to) {
            total += order->total();
        }
    }
    return total;
}

int OrderManager::getTotalOrderCount() const {
    return m_orders.size();
}

int OrderManager::getOrderCountByStatus(Order::Status status) const {
    int count = 0;
    for (const auto& order : m_orders) {
        if (order->status() == status) {
            count++;
        }
    }
    return count;
}

double OrderManager::getAverageOrderValue() const {
    if (m_orders.isEmpty()) return 0;
    return getTotalRevenue() / m_orders.size();
}

bool OrderManager::saveToFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << filename;
        return false;
    }

    QJsonArray ordersArray;
    for (const auto& order : m_orders) {
        QJsonObject orderObj;
        orderObj["id"] = order->id();
        orderObj["customerId"] = order->customerId();
        orderObj["customerName"] = order->customerName();
        orderObj["orderDate"] = order->orderDate().toString(Qt::ISODate);
        orderObj["status"] = Order::statusToString(order->status());

        QJsonArray itemsArray;
        for (const auto& item : order->items()) {
            QJsonObject itemObj;
            itemObj["productName"] = item.productName;
            itemObj["quantity"] = item.quantity;
            itemObj["price"] = item.price;
            itemsArray.append(itemObj);
        }
        orderObj["items"] = itemsArray;

        ordersArray.append(orderObj);
    }

    QJsonDocument doc(ordersArray);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool OrderManager::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filename;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format in file:" << filename;
        return false;
    }

    m_orders.clear();
    QJsonArray ordersArray = doc.array();

    for (const QJsonValue& value : ordersArray) {
        if (!value.isObject()) continue;

        QJsonObject orderObj = value.toObject();
        auto order = QSharedPointer<Order>::create(orderObj["customerId"].toString());
        order->setCustomerName(orderObj["customerName"].toString());
        order->setStatus(Order::stringToStatus(orderObj["status"].toString()));

        QJsonArray itemsArray = orderObj["items"].toArray();
        for (const QJsonValue& itemValue : itemsArray) {
            if (!itemValue.isObject()) continue;

            QJsonObject itemObj = itemValue.toObject();
            OrderItem item;
            item.productName = itemObj["productName"].toString();
            item.quantity = itemObj["quantity"].toInt();
            item.price = itemObj["price"].toDouble();
            order->addItem(item);
        }

        m_orders.append(order);
    }

    emit dataChanged();
    return true;
}
