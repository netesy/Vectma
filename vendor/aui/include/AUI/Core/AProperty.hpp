#pragma once
#include <functional>
#include <vector>

namespace aui {

template <typename T>
class AProperty {
public:
    AProperty(T initialValue) : m_value(initialValue) {}

    const T& get() const { return m_value; }

    void set(T newValue) {
        if (m_value != newValue) {
            m_value = newValue;
            notify();
        }
    }

    void operator=(T newValue) { set(newValue); }
    operator T() const { return get(); }

    void onChange(std::function<void(const T&)> callback) {
        m_listeners.push_back(callback);
    }

private:
    T m_value;
    std::vector<std::function<void(const T&)>> m_listeners;

    void notify() {
        for (auto& listener : m_listeners) {
            listener(m_value);
        }
    }
};

} // namespace aui
