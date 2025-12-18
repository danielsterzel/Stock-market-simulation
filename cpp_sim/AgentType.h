#pragma once


enum class AgentType {
    AGGRESSIVE,
    CONSERVATIVE,
    MOMENTUM,
    RANDOM
};

inline std::ostream &operator<<(std::ostream &os, const AgentType &type) {
    switch (type) {
        case AgentType::AGGRESSIVE:
            os << "AGGRESSIVE";
            break;
        case AgentType::CONSERVATIVE:
            os << "CONSERVATIVE";
            break;
        case AgentType::RANDOM:
            os << "RANDOM";
            break;
        default:
            os << "UNKNOWN";
    }
    return os;
}
