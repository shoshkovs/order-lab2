#include "Console.h"

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
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const std::vector<std::string> categories = {"Book", "Laptop", "Dumbbell", "Mug"};
const std::vector<std::string> regions = {"Saint Petersburg", "Moscow", "Vladivostok"};

bool readInteger(const std::string& prompt, int& value, int minimum, int maximum) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) {
            return false;
        }
        std::istringstream input(line);
        int number;
        char extra;
        if ((input >> number) && !(input >> extra) &&
            number >= minimum && number <= maximum) {
            value = number;
            return true;
        }
        std::cout << "Enter an integer from " << minimum
                  << " to " << maximum << ".\n";
    }
}

bool readName(std::string& name) {
    while (true) {
        std::cout << "Customer: ";
        if (!std::getline(std::cin, name)) {
            return false;
        }
        std::size_t first = name.find_first_not_of(" \t\r");
        if (first != std::string::npos) {
            std::size_t last = name.find_last_not_of(" \t\r");
            name = name.substr(first, last - first + 1);
            return true;
        }
        std::cout << "Name cannot be empty.\n";
    }
}

void printOrder(const Order& order) {
    // строка собирается заранее, чтобы логи доставки не попали в ее середину
    std::ostringstream line;
    line << order;
    std::cout << line.str() << '\n';
    for (const std::shared_ptr<Product>& item : order.getItems()) {
        std::cout << "    item " << item->getId() << " | "
                  << item->getType().getCategory() << " x" << item->getQuantity()
                  << " | price " << item->getCurrentPrice() << '\n';
    }
}

void printOrders(const OrderManager& manager) {
    if (manager.getOrders().empty()) {
        std::cout << "No orders yet.\n";
        return;
    }
    for (const std::unique_ptr<Order>& order : manager.getOrders()) {
        printOrder(*order);
    }
}

bool readProduct(int& nextProductId, std::shared_ptr<Product>& product) {
    int type, quantity;
    std::cout << "1. Book 0.5 kg 800\n2. Laptop 2 kg 60000\n"
                 "3. Dumbbell 6 kg 3000\n4. Mug 0.3 kg 400\n";
    if (!readInteger("Product type: ", type, 1, 4) ||
        !readInteger("Quantity: ", quantity, 1, 1000)) {
        return false;
    }
    product = std::make_shared<Product>(
        nextProductId, ProductType::getType(categories[type - 1]), quantity);
    ++nextProductId;
    return true;
}

bool readOrder(OrderManager& manager, Order*& order) {
    int id;
    if (!readInteger("Order ID: ", id, 1, std::numeric_limits<int>::max())) {
        return false;
    }
    order = manager.findOrder(id);
    if (order == nullptr) {
        throw std::invalid_argument("Order not found");
    }
    return true;
}

bool createFromConsole(OrderManager& manager, int& nextProductId) {
    std::string customer;
    int region;
    if (!readName(customer)) {
        return false;
    }
    std::cout << "1. Saint Petersburg\n2. Moscow\n3. Vladivostok (no courier and post)\n";
    if (!readInteger("Region: ", region, 1, 3)) {
        return false;
    }

    std::vector<std::shared_ptr<Product>> items;
    int more = 1;
    while (more == 1) {
        std::shared_ptr<Product> product;
        if (!readProduct(nextProductId, product)) {
            return false;
        }
        items.push_back(product);
        if (!readInteger("Add another product? (1=yes, 0=no): ", more, 0, 1)) {
            return false;
        }
    }
    Order& order = manager.createOrder(customer, items, regions[region - 1]);
    std::cout << "Created order " << order.getId() << ".\n";
    return true;
}

bool editItemsFromConsole(OrderManager& manager, int& nextProductId) {
    Order* order = nullptr;
    int action;
    if (!readOrder(manager, order)) {
        return false;
    }
    std::cout << "1. Add product\n2. Remove product\n";
    if (!readInteger("Action: ", action, 1, 2)) {
        return false;
    }
    if (action == 1) {
        std::shared_ptr<Product> product;
        if (!readProduct(nextProductId, product)) {
            return false;
        }
        order->addItem(product);
        std::cout << "Added product " << product->getId() << ".\n";
        return true;
    }
    int productId;
    if (!readInteger("Product ID: ", productId, 1, std::numeric_limits<int>::max())) {
        return false;
    }
    if (!order->removeItem(productId)) {
        throw std::invalid_argument("Product not found in this order");
    }
    std::cout << "Removed product " << productId << ".\n";
    return true;
}

bool discountFromConsole(OrderManager& manager, int& nextProductId) {
    Order* order = nullptr;
    int type;
    if (!readOrder(manager, order)) {
        return false;
    }
    std::cout << "1. Percent 10%\n2. Cashback 5%\n3. Free delivery\n4. Gift mug\n";
    if (!readInteger("Discount: ", type, 1, 4)) {
        return false;
    }
    switch (type) {
        case 1: *order + PercentDiscount(10); break;
        case 2: *order + CashbackDiscount(5); break;
        case 3: *order + FreeDeliveryDiscount(); break;
        case 4:
            *order + GiftDiscount(nextProductId, "Mug");
            ++nextProductId;
            break;
    }
    printOrder(*order);
    return true;
}

bool deliveryFromConsole(OrderManager& manager,
                         const std::vector<std::shared_ptr<Delivery>>& deliveries) {
    Order* order = nullptr;
    int type;
    if (!readOrder(manager, order)) {
        return false;
    }
    std::cout << "1. Courier 500, 1-2 days, up to 20 kg\n"
                 "2. Pickup free, 1-3 days\n"
                 "3. Post 200, 5-10 days, up to 10 kg\n";
    if (!readInteger("Delivery: ", type, 1, 3)) {
        return false;
    }
    Delivery& delivery = *deliveries[type - 1];
    delivery << *order;
    std::cout << "Delivery " << delivery.getName() << ", "
              << delivery.getEstimatedTime() << ".\n";
    printOrder(*order);
    return true;
}

bool statusFromConsole(OrderManager& manager) {
    Order* order = nullptr;
    int status;
    if (!readOrder(manager, order)) {
        return false;
    }
    std::cout << "Current status: " << toString(order->getStatus()) << '\n'
              << "1. Paid\n2. Shipped\n3. Delivered\n4. Cancelled\n";
    if (!readInteger("New status: ", status, 1, 4)) {
        return false;
    }
    const OrderStatus statuses[] = {OrderStatus::Paid, OrderStatus::Shipped,
                                    OrderStatus::Delivered, OrderStatus::Cancelled};
    manager.updateStatus(order->getId(), statuses[status - 1]);
    std::cout << "Order " << order->getId() << " is "
              << toString(order->getStatus()) << ".\n";
    return true;
}

void printStatistics(const OrderManager& manager) {
    std::cout << "Turnover: " << manager.getTurnover() << '\n'
              << "Customers: " << manager.getCustomerCount() << '\n';
    std::set<std::string> customers;
    for (const std::unique_ptr<Order>& order : manager.getOrders()) {
        customers.insert(order->getCustomer());
    }
    for (const std::string& customer : customers) {
        std::cout << "  " << customer << ": active orders "
                  << manager.getActiveOrders(customer) << '\n';
    }
}

int runConsole() {
    ProductType::getType("Book", 0.5, 800.0);
    ProductType::getType("Laptop", 2.0, 60000.0);
    ProductType::getType("Dumbbell", 6.0, 3000.0);
    ProductType::getType("Mug", 0.3, 400.0);

    // доставка через прокси, он проверяет регион и кеширует стоимость
    const std::vector<std::string> served = {"Saint Petersburg", "Moscow"};
    const std::vector<std::shared_ptr<Delivery>> deliveries = {
        std::make_shared<DeliveryProxy>(std::make_shared<CourierDelivery>(), served),
        std::make_shared<PickupDelivery>(),
        std::make_shared<DeliveryProxy>(std::make_shared<PostDelivery>(), served),
    };

    int limit;
    std::cout << "Order menu\nData is kept only until exit.\n";
    if (!readInteger("Active order limit per customer: ", limit, 1, 100)) {
        return 0;
    }
    OrderManager manager(limit);
    int nextProductId = 1;
    bool running = true;
    while (running) {
        std::cout << "\nOrders: " << manager.getOrders().size()
                  << "\n1. Show orders\n2. Create order\n3. Add or remove product\n"
                     "4. Apply discount\n5. Assign delivery\n6. Change status\n"
                     "7. Statistics\n0. Exit\n";
        int command;
        if (!readInteger("Command: ", command, 0, 7)) {
            break;
        }
        try {
            switch (command) {
                case 0: running = false; break;
                case 1: printOrders(manager); break;
                case 2: running = createFromConsole(manager, nextProductId); break;
                case 3: running = editItemsFromConsole(manager, nextProductId); break;
                case 4: running = discountFromConsole(manager, nextProductId); break;
                case 5: running = deliveryFromConsole(manager, deliveries); break;
                case 6: running = statusFromConsole(manager); break;
                case 7: printStatistics(manager); break;
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}
