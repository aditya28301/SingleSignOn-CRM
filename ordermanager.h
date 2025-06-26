#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include "order.h"

class OrderManager : public QObject {
    Q_OBJECT

public:
    explicit OrderManager(QObject *parent = nullptr);
    ~OrderManager();

    // Order operations
    void addOrder(QSharedPointer<Order> order);
    void updateOrderStatus(const QString& id, Order::Status status);
    void deleteOrder(const QString& id);

    // Query methods
    QList<QSharedPointer<Order>> getAllOrders() const;
    QList<QSharedPointer<Order>> getOrdersByCustomer(const QString& customerId) const;
    QList<QSharedPointer<Order>> getOrdersByStatus(Order::Status status) const;
    QList<QSharedPointer<Order>> getOrdersByDateRange(const QDateTime& from, const QDateTime& to) const;
    QSharedPointer<Order> getOrder(const QString& id);

    // Statistics methods
    double getTotalRevenue() const;
    double getRevenueByCustomer(const QString& customerId) const;
    double getRevenueByDateRange(const QDateTime& from, const QDateTime& to) const;
    int getTotalOrderCount() const;
    int getOrderCountByStatus(Order::Status status) const;
    double getAverageOrderValue() const;

    // Persistence
    bool saveToFile(const QString& filename) const;
    bool loadFromFile(const QString& filename);

    // Search
    QList<QSharedPointer<Order>> searchOrders(const QString& searchText) const;

signals:
    void orderAdded(const QSharedPointer<Order>& order);
    void orderUpdated(const QSharedPointer<Order>& order);
    void orderDeleted(const QString& id);
    void dataChanged();

private:
    QList<QSharedPointer<Order>> m_orders;

    void loadSampleOrders();
    int findOrderIndex(const QString& id) const;
};

#endif // ORDERMANAGER_H
