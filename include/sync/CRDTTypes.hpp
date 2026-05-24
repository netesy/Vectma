#pragma once
#include <cstdint>
#include <compare>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <string>
#include <optional>
#include <variant>

namespace vectma {

using ClientId = uint32_t;

struct LamportTimestamp {
    uint64_t timestamp = 0;
    ClientId clientId = 0;
    uint32_t counter = 0;

    auto operator<=>(const LamportTimestamp&) const = default;
    bool operator==(const LamportTimestamp&) const = default;
};

template <typename T>
struct LWWProperty {
    T value;
    std::optional<std::string> tokenPath;
    LamportTimestamp lastUpdate;

    LWWProperty() : value(), tokenPath(std::nullopt), lastUpdate{0, 0, 0} {}
    LWWProperty(const T& v, LamportTimestamp t) : value(v), tokenPath(std::nullopt), lastUpdate(t) {}

    bool update(const T& newValue, LamportTimestamp newTimestamp) {
        if (newTimestamp >= lastUpdate) {
            value = newValue;
            tokenPath = std::nullopt;
            lastUpdate = newTimestamp;
            return true;
        }
        return false;
    }

    bool bindToken(const std::string& path, LamportTimestamp newTimestamp) {
        if (newTimestamp >= lastUpdate) {
            tokenPath = path;
            lastUpdate = newTimestamp;
            return true;
        }
        return false;
    }
};

using NodeId = LamportTimestamp;

class LamportClock {
public:
    static LamportClock& getInstance() { static LamportClock instance; return instance; }
    void setClientId(ClientId id) { m_clientId = id; }
    ClientId getClientId() const { return m_clientId; }

    LamportTimestamp tick() {
        uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        uint32_t count = m_counter.fetch_add(1, std::memory_order_relaxed);
        return {now, m_clientId, count};
    }

    void merge(LamportTimestamp remote) {
        uint32_t current = m_counter.load(std::memory_order_relaxed);
        while (remote.counter >= current && !m_counter.compare_exchange_weak(current, remote.counter + 1));
    }

private:
    LamportClock() : m_clientId(0), m_counter(0) {}
    ClientId m_clientId;
    std::atomic<uint32_t> m_counter;
};

} // namespace vectma
