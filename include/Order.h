#pragma once

#include "IObservable.h"
#include "OrderStatus.h"
#include "Product.h"

#include <memory>
#include <string>
#include <vector>

class Delivery;
class Discount;

class Order : public IObservable {
public:
    Order(int id, const std::string& customer, const std::string& region);
    Order(const Order& other);
    Order& operator=(const Order& other);
    Order(Order&& other) noexcept = default;
    Order& operator=(Order&& other) noexcept = default;
    ~Order() override = default;

    int getId() const;
    const std::string& getCustomer() const;
    const std::string& getRegion() const;
    OrderStatus getStatus() const;
    const std::vector<std::shared_ptr<Product>>& getItems() const;
    const std::vector<std::string>& getDiscounts() const;
    std::shared_ptr<Delivery> getDelivery() const;

    void addItem(const std::shared_ptr<Product>& item);
    bool removeItem(int productId);

    double getTotalWeight() const;
    double getItemsPrice() const;
    double getDeliveryCost() const;
    double getTotalPrice() const;

    void setStatus(OrderStatus newStatus);
    void applyDiscount(const Discount& discount);
    void setDelivery(const std::shared_ptr<Delivery>& delivery);

    void setFreeDelivery(bool freeDelivery);
    bool hasFreeDelivery() const;
    void addBonus(double amount);
    double getBonus() const;

private:
    void requirePending(const std::string& operation) const;

    int id_;
    std::string customer_;
    std::string region_;
    std::vector<std::shared_ptr<Product>> items_;
    OrderStatus status_;
    std::shared_ptr<Delivery> delivery_;
    std::vector<std::string> discounts_;
    bool freeDelivery_;
    double bonus_;
};
