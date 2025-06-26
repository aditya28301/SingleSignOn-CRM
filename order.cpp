#include "order.h"
#include <QUuid>

Order::Order()
    : m_id(generateId()), m_orderDate(QDateTime::currentDateTime()), m_status(Pending)
{
}

Order::Order(const QString& customerId)
    : m_id(generateId()), m_customerId(customerId), m_orderDate(QDateTime::currentDateTime()), m_status(Pending)
{
}

void Order::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
    }
}

double Order::total() const {
    double sum = 0.0;
    for (const auto& item : m_items) {
        sum += item.total();
    }
    return sum;
}

QString Order::statusToString(Status status) {
    switch (status) {
    case Pending: return "Pending";
    case Processing: return "Processing";
    case Shipped: return "Shipped";
    case Delivered: return "Delivered";
    case Cancelled: return "Cancelled";
    }
    return "Unknown";
}

Order::Status Order::stringToStatus(const QString& str) {
    if (str == "Pending") return Pending;
    if (str == "Processing") return Processing;
    if (str == "Shipped") return Shipped;
    if (str == "Delivered") return Delivered;
    if (str == "Cancelled") return Cancelled;
    return Pending;
}

QString Order::generateId() {
    return QString("ORD-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8).toUpper());
}
