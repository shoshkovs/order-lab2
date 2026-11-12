#include "Order.h"

#include "Delivery.h"
#include "Discount.h"
#include "LoggingProxy.h"

#include <algorithm>
#include <stdexcept>

Order::Order(int id, const std::string& customer, const std::string& region)
    : id_(id),
      customer_(customer),
      region_(region),
      status_(OrderStatus::Pending),
      freeDelivery_(false),
      bonus_(0.0) {
    if (id <= 0) {
        throw std::invalid_argument("Order ID must be positive");
    }
    if (customer.empty()) {
        throw std::invalid_argument("Customer cannot be empty");
    }
}

Order::Order(const Order& other)
    : IObservable(),
      id_(other.id_),
      customer_(other.customer_),
      region_(other.region_),
      status_(other.status_),
      delivery_(other.delivery_),
      discounts_(other.discounts_),
      freeDelivery_(other.freeDelivery_),
      bonus_(other.bonus_) {
    for (const std::shared_ptr<Product>& item : other.items_) {
        // отдельный товар, иначе скидка на копии изменит цену в оригинале
        items_.push_back(std::make_shared<Product>(*item));
    }
}

Order& Order::operator=(const Order& other) {
    if (this == &other) {
        return *this;
    }
    // сначала делаем глубокую копию, чтобы при ошибке старые данные остались
    Order copy(other);
    id_ = copy.id_;
    customer_ = std::move(copy.customer_);
    region_ = std::move(copy.region_);
    items_ = std::move(copy.items_);
    status_ = copy.status_;
    delivery_ = std::move(copy.delivery_);
    discounts_ = std::move(copy.discounts_);
    freeDelivery_ = copy.freeDelivery_;
    bonus_ = copy.bonus_;
    // подписчики остаются свои, базовый класс не меняем
    return *this;
}

int Order::getId() const {
    return id_;
}

const std::string& Order::getCustomer() const {
    return customer_;
}

const std::string& Order::getRegion() const {
    return region_;
}

OrderStatus Order::getStatus() const {
    return status_;
}

const std::vector<std::shared_ptr<Product>>& Order::getItems() const {
    return items_;
}

const std::vector<std::string>& Order::getDiscounts() const {
    return discounts_;
}

std::shared_ptr<Delivery> Order::getDelivery() const {
    return delivery_;
}

void Order::addItem(const std::shared_ptr<Product>& item) {
    requirePending("add item");
    if (item == nullptr) {
        throw std::invalid_argument("Item cannot be null");
    }
    items_.push_back(item);
    LoggingProxy::log("ORDER", "add item", item->getType().getCategory());
}

bool Order::removeItem(int productId) {
    requirePending("remove item");
    auto found = std::find_if(items_.begin(), items_.end(),
                              [productId](const std::shared_ptr<Product>& item) {
                                  return item->getId() == productId;
                              });
    if (found == items_.end()) {
        return false;
    }
    items_.erase(found);
    LoggingProxy::log("ORDER", "remove item", std::to_string(productId));
    return true;
}

double Order::getTotalWeight() const {
    double total = 0.0;
    for (const std::shared_ptr<Product>& item : items_) {
        total += item->getWeight();
    }
    return total;
}

double Order::getItemsPrice() const {
    double total = 0.0;
    for (const std::shared_ptr<Product>& item : items_) {
        total += item->getTotalPrice();
    }
    return total;
}

double Order::getDeliveryCost() const {
    if (delivery_ == nullptr || freeDelivery_) {
        return 0.0;
    }
    return delivery_->getCost(*this);
}

double Order::getTotalPrice() const {
    return getItemsPrice() + getDeliveryCost();
}

void Order::setStatus(OrderStatus newStatus) {
    const bool allowed =
        (status_ == OrderStatus::Pending &&
         (newStatus == OrderStatus::Paid || newStatus == OrderStatus::Cancelled)) ||
        (status_ == OrderStatus::Paid &&
         (newStatus == OrderStatus::Shipped || newStatus == OrderStatus::Cancelled)) ||
        (status_ == OrderStatus::Shipped && newStatus == OrderStatus::Delivered);
    if (!allowed) {
        throw std::logic_error("Cannot change status from " + toString(status_) +
                               " to " + toString(newStatus));
    }
    if (newStatus == OrderStatus::Shipped && delivery_ == nullptr) {
        throw std::logic_error("Delivery is not assigned");
    }

    const OrderStatus oldStatus = status_;
    status_ = newStatus;
    LoggingProxy::log("ORDER", "status",
                      toString(oldStatus) + " -> " + toString(newStatus));
    notify(OrderEvent(id_, oldStatus, newStatus));
}

void Order::applyDiscount(const Discount& discount) {
    requirePending("apply discount");
    discount.apply(*this);
    discounts_.push_back(discount.getName());
    LoggingProxy::log("DISCOUNT", "apply", discount.getName());
}

void Order::setDelivery(const std::shared_ptr<Delivery>& delivery) {
    requirePending("set delivery");
    if (delivery == nullptr) {
        throw std::invalid_argument("Delivery cannot be null");
    }
    if (!delivery->isAvailable(*this)) {
        throw std::invalid_argument("Delivery is not available for this order");
    }
    delivery_ = delivery;
    LoggingProxy::log("DELIVERY", "assign", delivery->getName());
}

void Order::setFreeDelivery(bool freeDelivery) {
    freeDelivery_ = freeDelivery;
}

bool Order::hasFreeDelivery() const {
    return freeDelivery_;
}

void Order::addBonus(double amount) {
    if (amount < 0.0) {
        throw std::invalid_argument("Bonus cannot be negative");
    }
    bonus_ += amount;
}

double Order::getBonus() const {
    return bonus_;
}

void Order::requirePending(const std::string& operation) const {
    if (status_ != OrderStatus::Pending) {
        throw std::logic_error("Cannot " + operation + " after payment");
    }
}
