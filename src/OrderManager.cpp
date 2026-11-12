#include "OrderManager.h"

#include "LoggingProxy.h"

#include <stdexcept>

OrderManager::OrderManager(int maxActiveOrders)
    : turnover_(0.0), maxActiveOrders_(maxActiveOrders), nextOrderId_(1) {
    if (maxActiveOrders <= 0) {
        throw std::invalid_argument("Active order limit must be positive");
    }
}

Order& OrderManager::createOrder(const std::string& customer,
                                 const std::vector<std::shared_ptr<Product>>& items,
                                 const std::string& region) {
    if (getActiveOrders(customer) >= maxActiveOrders_) {
        LoggingProxy::log("MANAGER", "create order", "limit for " + customer);
        throw std::overflow_error("Active order limit reached for " + customer);
    }

    std::unique_ptr<Order> order =
        std::make_unique<Order>(nextOrderId_, customer, region);
    for (const std::shared_ptr<Product>& item : items) {
        order->addItem(item);
    }
    order->subscribe(this);
    ++nextOrderId_;

    customers_.insert(customer);
    ++activeOrders_[customer];
    LoggingProxy::log("MANAGER", "create order",
                      std::to_string(order->getId()) + " for " + customer);
    orders_.push_back(std::move(order));
    return *orders_.back();
}

void OrderManager::updateStatus(int orderId, OrderStatus newStatus) {
    Order* order = findOrder(orderId);
    if (order == nullptr) {
        throw std::out_of_range("Order not found");
    }
    // остальное сделает onOrderEvent после уведомления
    order->setStatus(newStatus);
}

Order* OrderManager::findOrder(int orderId) {
    for (const std::unique_ptr<Order>& order : orders_) {
        if (order->getId() == orderId) {
            return order.get();
        }
    }
    return nullptr;
}

double OrderManager::getTurnover() const {
    return turnover_;
}

std::size_t OrderManager::getCustomerCount() const {
    return customers_.size();
}

int OrderManager::getActiveOrders(const std::string& customer) const {
    auto found = activeOrders_.find(customer);
    return found == activeOrders_.end() ? 0 : found->second;
}

void OrderManager::onOrderEvent(const OrderEvent& event) {
    Order* order = findOrder(event.getOrderId());
    if (order == nullptr) {
        return;
    }
    const std::string& customer = order->getCustomer();

    if (event.getNewStatus() == OrderStatus::Paid) {
        const double amount = order->getTotalPrice();
        paidAmounts_[order->getId()] = amount;
        turnover_ += amount;
    }

    if (event.getNewStatus() == OrderStatus::Cancelled) {
        // если заказ уже оплатили, деньги возвращаются и оборот уменьшается
        auto paid = paidAmounts_.find(order->getId());
        if (paid != paidAmounts_.end()) {
            turnover_ -= paid->second;
            paidAmounts_.erase(paid);
        }
    }

    if (event.getNewStatus() == OrderStatus::Cancelled ||
        event.getNewStatus() == OrderStatus::Delivered) {
        if (activeOrders_[customer] <= 0) {
            throw std::logic_error("Active order counter is broken");
        }
        --activeOrders_[customer];
    }

    LoggingProxy::log("MANAGER", "order " + std::to_string(order->getId()),
                      toString(event.getOldStatus()) + " -> " +
                          toString(event.getNewStatus()) +
                          ", turnover " + std::to_string(turnover_) +
                          ", active " + std::to_string(activeOrders_[customer]));
}
