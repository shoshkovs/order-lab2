#pragma once

#include <stdexcept>
#include <string>

inline void check(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Exception, typename Action>
void expectThrows(Action action, const std::string& message) {
    bool caught = false;
    try {
        action();
    } catch (const Exception&) {
        caught = true;
    }
    check(caught, "Expected exception: " + message);
}
