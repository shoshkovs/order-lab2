#include "IObservable.h"

#include <algorithm>
#include <stdexcept>

void IObservable::subscribe(IObserver* observer) {
    if (observer == nullptr) {
        throw std::invalid_argument("Observer cannot be null");
    }
    if (std::find(observers_.begin(), observers_.end(), observer) ==
        observers_.end()) {
        observers_.push_back(observer);
    }
}

void IObservable::unsubscribe(IObserver* observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                     observers_.end());
}

void IObservable::notify(const OrderEvent& event) const {
    for (IObserver* observer : observers_) {
        observer->onOrderEvent(event);
    }
}
