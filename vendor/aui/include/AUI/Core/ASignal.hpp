#pragma once
#include <functional>
#include <vector>
#include "AUI/Core/AProperty.hpp"

namespace aui {

template <typename... Args>
class ASignal {
public:
    void connect(std::function<void(Args...)> slot) {
        m_slots.push_back(slot);
    }

    void operator()(Args... args) {
        for (auto& slot : m_slots) {
            slot(args...);
        }
    }

private:
    std::vector<std::function<void(Args...)>> m_slots;
};

template <typename T, typename Slot>
void connect(AProperty<T>& property, Slot slot) {
    property.onChange(slot);
}

} // namespace aui
