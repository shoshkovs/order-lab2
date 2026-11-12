#include "TestHarness.h"

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

#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const std::vector<std::string> regions = {"Saint Petersburg"};

bool same(double left, double right) {
    return std::fabs(left - right) < 1e-9;
}

std::shared_ptr<Product> makeProduct(int id, const std::string& category,
                                     int quantity) {
    return std::make_shared<Product>(id, ProductType::getType(category), quantity);
}

void testFlyweightSharesProductTypes() {
    std::shared_ptr<ProductType> first = ProductType::getType("Book", 0.5, 800.0);
    std::shared_ptr<ProductType> second = ProductType::getType("Book", 9.0, 1.0);
    check(first == second, "same category returns the same object");
    check(same(second->getBasePrice(), 800.0), "first registration is kept");
    check(ProductType::getType("Book") == first, "lookup by category");
    expectThrows<std::out_of_range>([] { ProductType::getType("Unknown"); },
                                    "unknown type");

    Product book(1, first, 3);
    check(same(book.getWeight(), 1.5), "weight = base weight * quantity");
    check(same(book.getTotalPrice(), 2400.0), "price = current price * quantity");
    book.changeQuantity(-1);
    check(book.getQuantity() == 2, "quantity decreased");
    expectThrows<std::invalid_argument>([&book] { book.changeQuantity(-2); },
                                        "quantity cannot become zero");
}

void testDeliveryStrategiesAndProxy() {
    ProductType::getType("Dumbbell", 6.0, 3000.0);
    Order light(1, "Anna", "Saint Petersburg");
    light.addItem(makeProduct(1, "Book", 2));
    Order heavy(2, "Anna", "Saint Petersburg");
    heavy.addItem(makeProduct(2, "Dumbbell", 2));
    Order remote(3, "Anna", "Vladivostok");
    remote.addItem(makeProduct(3, "Book", 1));

    CourierDelivery courier;
    PickupDelivery pickup;
    PostDelivery post;
    check(same(courier.getCost(light), 500.0), "courier cost");
    check(same(pickup.getCost(heavy), 0.0), "pickup is free");
    check(same(post.getCost(light), 200.0), "post cost");
    check(!post.isAvailable(heavy), "post limit 10 kg");
    check(courier.isAvailable(heavy), "courier limit 20 kg");
    expectThrows<std::invalid_argument>([&] { post.getCost(heavy); },
                                        "post cost for heavy order");

    DeliveryProxy proxy(std::make_shared<CourierDelivery>(), regions);
    check(!proxy.isAvailable(remote), "proxy checks region");
    check(proxy.isAvailable(light), "proxy delegates to courier");
    check(same(proxy.getCost(light), 500.0), "proxy returns real cost");
    proxy.getCost(light);
    check(proxy.getRealCostCalls() == 1, "second request is cached");
    light.addItem(makeProduct(4, "Book", 1));
    proxy.getCost(light);
    check(proxy.getRealCostCalls() == 2, "new weight resets cache");
    check(proxy.getEstimatedTime() == "1-2 days", "proxy delegates time");
}

void testDiscountStrategies() {
    ProductType::getType("Mug", 0.3, 400.0);
    std::shared_ptr<CourierDelivery> courier = std::make_shared<CourierDelivery>();
    Order order(1, "Boris", "Saint Petersburg");
    order.addItem(makeProduct(1, "Book", 5));
    order.setDelivery(courier);
    check(same(order.getTotalPrice(), 4500.0), "4000 goods + 500 delivery");

    order.applyDiscount(PercentDiscount(10));
    check(same(order.getItemsPrice(), 3600.0), "10 percent off");
    order.applyDiscount(CashbackDiscount(5));
    check(same(order.getBonus(), 180.0), "5 percent to bonus");
    check(same(order.getItemsPrice(), 3600.0), "cashback keeps price");
    order.applyDiscount(FreeDeliveryDiscount());
    check(same(order.getDeliveryCost(), 0.0), "delivery became free");
    order.applyDiscount(GiftDiscount(50, "Mug"));
    check(order.getItems().size() == 2, "gift added");
    check(same(order.getItemsPrice(), 3600.0), "gift is free");
    check(order.getDiscounts().size() == 4, "four discounts recorded");

    expectThrows<std::invalid_argument>([] { PercentDiscount(150); },
                                        "percent above 100");
}

void testObserverUpdatesManager() {
    std::shared_ptr<PickupDelivery> pickup = std::make_shared<PickupDelivery>();
    OrderManager manager(2);
    Order& first = manager.createOrder("Vera", {makeProduct(1, "Book", 2)});
    Order& second = manager.createOrder("Vera", {makeProduct(2, "Book", 1)});
    manager.createOrder("Gleb", {makeProduct(3, "Book", 1)});
    check(manager.getCustomerCount() == 2, "two unique customers");
    expectThrows<std::overflow_error>(
        [&] { manager.createOrder("Vera", {makeProduct(4, "Book", 1)}); },
        "active order limit");

    first.setDelivery(pickup);
    manager.updateStatus(first.getId(), OrderStatus::Paid);
    check(same(manager.getTurnover(), 1600.0), "paid order adds turnover");
    expectThrows<std::logic_error>(
        [&] { manager.updateStatus(first.getId(), OrderStatus::Delivered); },
        "paid cannot jump to delivered");
    expectThrows<std::logic_error>(
        [&] { manager.updateStatus(second.getId(), OrderStatus::Paid);
              manager.updateStatus(second.getId(), OrderStatus::Shipped); },
        "cannot ship without delivery");

    manager.updateStatus(first.getId(), OrderStatus::Cancelled);
    check(same(manager.getTurnover(), 800.0), "refund of cancelled order");
    check(manager.getActiveOrders("Vera") == 1, "cancel frees the limit");
    manager.createOrder("Vera", {makeProduct(5, "Book", 1)});
    check(manager.getActiveOrders("Vera") == 2, "new order after cancel");
}

void testOperatorsAndOrderCopy() {
    std::shared_ptr<Delivery> post = std::make_shared<PostDelivery>();
    Order order(1, "Dina", "Saint Petersburg");
    order.addItem(makeProduct(1, "Book", 2));

    *post << order;
    check(order.getDelivery() == post, "delivery << order");
    PercentDiscount percent(10);
    FreeDeliveryDiscount freeDelivery;
    order + percent + freeDelivery;
    check(same(order.getTotalPrice(), 1440.0), "order + discount + discount");

    std::ostringstream output;
    output << order;
    check(output.str().find("Order 1 | Dina | pending") == 0, "ostream << order");

    Order copy(order);
    copy.applyDiscount(PercentDiscount(50));
    check(same(order.getItemsPrice(), 1440.0), "original price unchanged");
    check(same(copy.getItemsPrice(), 720.0), "copy has own products");

    PostDelivery local;
    expectThrows<std::bad_weak_ptr>([&] { local << order; },
                                    "delivery must be owned by shared_ptr");
    order.setStatus(OrderStatus::Paid);
    expectThrows<std::logic_error>([&] { order + percent; },
                                   "no discounts after payment");
}

int main() {
    ProductType::getType("Book", 0.5, 800.0);

    const std::vector<std::pair<std::string, void (*)()>> tests = {
        {"testFlyweightSharesProductTypes", testFlyweightSharesProductTypes},
        {"testDeliveryStrategiesAndProxy", testDeliveryStrategiesAndProxy},
        {"testDiscountStrategies", testDiscountStrategies},
        {"testObserverUpdatesManager", testObserverUpdatesManager},
        {"testOperatorsAndOrderCopy", testOperatorsAndOrderCopy},
    };

    int failures = 0;
    for (const auto& test : tests) {
        try {
            test.second();
            std::cout << "[PASS] " << test.first << '\n';
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "[FAIL] " << test.first << ": " << error.what() << '\n';
        }
    }

    std::cout << (static_cast<int>(tests.size()) - failures) << '/'
              << tests.size() << " tests passed\n";
    return failures == 0 ? 0 : 1;
}
