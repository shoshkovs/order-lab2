#pragma once

#include "OrderEvent.h"

class IObserver {
public:
    virtual ~IObserver() = default;

    virtual void onOrderEvent(const OrderEvent& event) = 0;
};
