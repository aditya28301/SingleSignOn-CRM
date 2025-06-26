#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QDateTime>
#include <QList>

struct OrderItem {
    QString productName;
    int quantity;
    double price;

    double total() const { return quantity * price; }
};

class Order : public QObject{
    Q_OBJECT
public:
    enum Status {
        Pending,
        Processing,
        Shipped,
        Delivered,
        Cancelled
    };

    Order();
    Order(const QString& customerId);

    QString id() const { return m_id; }
    QString customerId() const { return m_customerId; }
    QString customerName() const { return m_customerName; }
    QDateTime orderDate() const { return m_orderDate; }
    Status status() const { return m_status; }
    QList<OrderItem> items() const { return m_items; }

    void setCustomerName(const QString& name) { m_customerName = name; }
    void setStatus(Status status) { m_status = status; }
    void addItem(const OrderItem& item) { m_items.append(item); }
    void removeItem(int index);
    double total() const;

    static QString statusToString(Status status);
    static Status stringToStatus(const QString& str);

private:
    QString m_id;
    QString m_customerId;
    QString m_customerName;
    QDateTime m_orderDate;
    Status m_status;
    QList<OrderItem> m_items;

    QString generateId();
};

#endif // ORDER_H
