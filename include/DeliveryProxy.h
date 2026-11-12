#pragma once

#include "Delivery.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class DeliveryProxy : public Delivery {
public:
    DeliveryProxy(const std::shared_ptr<Delivery>& realDelivery,
                  const std::vector<std::string>& regions);

    double getCost(const Order& order) const override;
    std::string getEstimatedTime() const override;
    bool isAvailable(const Order& order) const override;
    std::string getName() const override;

    std::size_t getRealCostCalls() const;

private:
    struct CachedCost {
        double weight;
        double cost;
    };

    std::shared_ptr<Delivery> realDelivery_;
    std::vector<std::string> regions_;
    mutable std::unordered_map<int, CachedCost> cache_;
    mutable std::size_t realCostCalls_;
};
