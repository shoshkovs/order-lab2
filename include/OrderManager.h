#pragma once

#include "IObserver.h"
#include "Order.h"
#include "Product.h"

#include <cstddef>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class OrderManager : public IObserver {
public:
    explicit OrderManager(int maxActiveOrders);
    // менеджер подписан на заказы по адресу, копия или перенос сломали бы подписку
    OrderManager(const OrderManager& other) = delete;
    OrderManager& operator=(const OrderManager& other) = delete;
    OrderManager(OrderManager&& other) = delete;
    OrderManager& operator=(OrderManager&& other) = delete;
    ~OrderManager() override = default;

    Order& createOrder(const std::string& customer,
                       const std::vector<std::shared_ptr<Product>>& items,
                       const std::string& region = "Saint Petersburg");
    void updateStatus(int orderId, OrderStatus newStatus);
    Order* findOrder(int orderId);
    const std::vector<std::unique_ptr<Order>>& getOrders() const;

    double getTurnover() const;
    std::size_t getCustomerCount() const;
    int getActiveOrders(const std::string& customer) const;

    void onOrderEvent(const OrderEvent& event) override;

private:
    std::vector<std::unique_ptr<Order>> orders_;
    std::set<std::string> customers_;
    std::unordered_map<std::string, int> activeOrders_;
    std::unordered_map<int, double> paidAmounts_;
    double turnover_;
    int maxActiveOrders_;
    int nextOrderId_;
};
