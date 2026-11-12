#include "CashbackDiscount.h"
#include "CourierDelivery.h"
#include "DeliveryProxy.h"
#include "FreeDeliveryDiscount.h"
#include "GiftDiscount.h"
#include "Operators.h"
#include "OrderManager.h"
#include "PercentDiscount.h"
#include "PickupDelivery.h"
#include "PostDelivery.h"
#include "ProductType.h"

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

const std::vector<std::string> serviceRegions = {"Saint Petersburg", "Moscow"};

void registerTypes() {
    ProductType::getType("Book", 0.5, 800.0);
    ProductType::getType("Laptop", 2.0, 60000.0);
    ProductType::getType("Dumbbell", 6.0, 3000.0);
    ProductType::getType("Mug", 0.3, 400.0);
}

std::shared_ptr<Product> makeProduct(int id, const std::string& category,
                                     int quantity) {
    return std::make_shared<Product>(id, ProductType::getType(category), quantity);
}

void printOrder(const std::string& label, const Order& order) {
    std::ostringstream line;
    line << order;
    std::cout << "  " << label << line.str() << '\n';
}

void scenario1() {
    std::cout << "Scenario 1: create order and apply percent discount\n";

    OrderManager manager(5);
    Order& order = manager.createOrder(
        "Anna", {makeProduct(1, "Book", 2), makeProduct(2, "Laptop", 1)});
    printOrder("Before: ", order);

    PercentDiscount percent(10);
    order + percent;
    printOrder("After:  ", order);
}

void scenario2() {
    std::cout << "Scenario 2: choose delivery through proxy\n";

    std::shared_ptr<DeliveryProxy> post = std::make_shared<DeliveryProxy>(
        std::make_shared<PostDelivery>(), serviceRegions);
    std::shared_ptr<DeliveryProxy> courier = std::make_shared<DeliveryProxy>(
        std::make_shared<CourierDelivery>(), serviceRegions);

    OrderManager manager(5);
    Order& order = manager.createOrder("Boris", {makeProduct(3, "Dumbbell", 2)});
    std::cout << "  Weight: " << order.getTotalWeight() << " kg\n";

    try {
        *post << order;
    } catch (const std::exception& error) {
        std::cout << "  Post rejected: " << error.what() << '\n';
    }
    *courier << order;
    printOrder("Courier " + courier->getEstimatedTime() + ": ", order);
    order.getTotalPrice();
    std::cout << "  Cost requests: 3, real calculations: "
              << courier->getRealCostCalls() << '\n';

    Order& remote = manager.createOrder("Boris", {makeProduct(4, "Book", 1)},
                                        "Vladivostok");
    const bool remoteAvailable = courier->isAvailable(remote);
    std::cout << "  Courier to Vladivostok: " << (remoteAvailable ? "yes" : "no")
              << '\n';
}

void scenario3() {
    std::cout << "Scenario 3: status change notifies manager\n";

    std::shared_ptr<PickupDelivery> pickup = std::make_shared<PickupDelivery>();
    OrderManager manager(5);
    Order& order = manager.createOrder("Vera", {makeProduct(5, "Book", 3)});
    *pickup << order;

    std::cout << "  Turnover before: " << manager.getTurnover() << '\n';
    manager.updateStatus(order.getId(), OrderStatus::Paid);
    std::cout << "  Turnover after paid: " << manager.getTurnover() << '\n';
    manager.updateStatus(order.getId(), OrderStatus::Shipped);
    manager.updateStatus(order.getId(), OrderStatus::Delivered);
    std::cout << "  Active orders of Vera: " << manager.getActiveOrders("Vera") << '\n';
    printOrder("", order);
}

void scenario4() {
    std::cout << "Scenario 4: several discounts on one order\n";

    std::shared_ptr<DeliveryProxy> courier = std::make_shared<DeliveryProxy>(
        std::make_shared<CourierDelivery>(), serviceRegions);
    OrderManager manager(5);
    Order& order = manager.createOrder("Gleb", {makeProduct(6, "Book", 5)});
    *courier << order;
    printOrder("Before: ", order);

    PercentDiscount percent(10);
    CashbackDiscount cashback(5);
    FreeDeliveryDiscount freeDelivery;
    GiftDiscount gift(100, "Mug");
    order + percent + cashback + freeDelivery + gift;

    printOrder("After:  ", order);
    std::cout << "  Applied:";
    for (const std::string& name : order.getDiscounts()) {
        std::cout << " [" << name << ']';
    }
    std::cout << '\n';
}

void scenario5() {
    std::cout << "Scenario 5: cancel order and free active limit\n";

    std::shared_ptr<PickupDelivery> pickup = std::make_shared<PickupDelivery>();
    OrderManager manager(2);
    Order& first = manager.createOrder("Dina", {makeProduct(7, "Book", 1)});
    manager.createOrder("Dina", {makeProduct(8, "Mug", 1)});
    *pickup << first;
    manager.updateStatus(first.getId(), OrderStatus::Paid);
    std::cout << "  Active: " << manager.getActiveOrders("Dina")
              << ", turnover: " << manager.getTurnover() << '\n';

    try {
        manager.createOrder("Dina", {makeProduct(9, "Book", 1)});
    } catch (const std::exception& error) {
        std::cout << "  Third order rejected: " << error.what() << '\n';
    }

    manager.updateStatus(first.getId(), OrderStatus::Cancelled);
    std::cout << "  After cancel active: " << manager.getActiveOrders("Dina")
              << ", turnover: " << manager.getTurnover() << '\n';
    manager.createOrder("Dina", {makeProduct(10, "Book", 1)});
    manager.createOrder("Egor", {makeProduct(11, "Mug", 2)});
    std::cout << "  Active: " << manager.getActiveOrders("Dina")
              << ", customers: " << manager.getCustomerCount() << '\n';
}

int main() {
    try {
        registerTypes();
        scenario1();
        scenario2();
        scenario3();
        scenario4();
        scenario5();
        std::cout << "Product types in pool: " << ProductType::getPoolSize() << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}
