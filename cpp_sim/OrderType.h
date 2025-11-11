#pragma once

enum class OrderType {
    MARKETORDER,
    LIMITORDER,
    CANCELORDER
};

inline std::ostream &operator<<(std::ostream &os, const OrderType &type) {
    switch (type) {
        case OrderType::MARKETORDER:
            os << "MARKETORDER";
            break;
        case OrderType::LIMITORDER:
            os << "LIMITORDER";
            break;
        case OrderType::CANCELORDER:
            os << "CANCELORDER";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}
