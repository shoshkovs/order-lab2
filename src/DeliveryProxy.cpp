#include "DeliveryProxy.h"

#include "LoggingProxy.h"
#include "Order.h"

#include <algorithm>
#include <stdexcept>

DeliveryProxy::DeliveryProxy(const std::shared_ptr<Delivery>& realDelivery,
                             const std::vector<std::string>& regions)
    : realDelivery_(realDelivery), regions_(regions), realCostCalls_(0) {
    if (realDelivery == nullptr) {
        throw std::invalid_argument("Real delivery cannot be null");
    }
}

double DeliveryProxy::getCost(const Order& order) const {
    LoggingProxy::log("DELIVERY", getName() + " get cost",
                      "order " + std::to_string(order.getId()));
    if (!isAvailable(order)) {
        throw std::invalid_argument(getName() + " is not available for this order");
    }

    const double weight = order.getTotalWeight();
    auto cached = cache_.find(order.getId());
    // вес мог измениться после подарка или удаления товара
    if (cached != cache_.end() && cached->second.weight == weight) {
        LoggingProxy::log("DELIVERY", getName() + " cache", "hit");
        return cached->second.cost;
    }

    const double cost = realDelivery_->getCost(order);
    ++realCostCalls_;
    cache_[order.getId()] = CachedCost{weight, cost};
    LoggingProxy::log("DELIVERY", getName() + " cache", "miss");
    return cost;
}

std::string DeliveryProxy::getEstimatedTime() const {
    return realDelivery_->getEstimatedTime();
}

bool DeliveryProxy::isAvailable(const Order& order) const {
    if (order.getTotalWeight() <= 0.0) {
        LoggingProxy::log("DELIVERY", getName() + " available", "empty order");
        return false;
    }
    if (std::find(regions_.begin(), regions_.end(), order.getRegion()) ==
        regions_.end()) {
        LoggingProxy::log("DELIVERY", getName() + " available",
                          "no service in " + order.getRegion());
        return false;
    }
    const bool available = realDelivery_->isAvailable(order);
    LoggingProxy::log("DELIVERY", getName() + " available",
                      available ? "yes" : "too heavy");
    return available;
}

std::string DeliveryProxy::getName() const {
    return realDelivery_->getName();
}

std::size_t DeliveryProxy::getRealCostCalls() const {
    return realCostCalls_;
}
