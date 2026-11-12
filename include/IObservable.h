#pragma once

#include "IObserver.h"
#include "OrderEvent.h"

#include <vector>

class IObservable {
public:
    virtual ~IObservable() = default;

    void subscribe(IObserver* observer);
    void unsubscribe(IObserver* observer);
    void notify(const OrderEvent& event) const;

private:
    std::vector<IObserver*> observers_;
};
