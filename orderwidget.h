#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QFormLayout>

class OrderManager;

class OrderWidget : public QWidget {
    Q_OBJECT

public:
    explicit OrderWidget(OrderManager* orderManager, QWidget *parent = nullptr);

private slots:
    void createOrder();
    void updateOrderStatus();
    void deleteOrder();
    void refreshOrderList();
    void onOrderSelectionChanged();
    void viewOrderDetails();

private:
    void setupUi();
    void loadOrders();
    void updateStatistics();

    OrderManager* m_orderManager;
    QTableWidget* m_orderTable;
    QComboBox* m_customerFilter;
    QComboBox* m_statusCombo;
    QPushButton* m_createBtn;
    QPushButton* m_updateBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_viewBtn;

    // Statistics labels
    QLabel* m_totalRevenueLabel;
    QLabel* m_orderCountLabel;
    QLabel* m_avgOrderLabel;
};

#endif // ORDERWIDGET_H

