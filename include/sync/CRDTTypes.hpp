#pragma once
#include <cstdint>
#include <compare>
#include <chrono>
#include <atomic>
#include <algorithm>
namespace vectma {
using ClientId = uint32_t;
struct LamportTimestamp {
    uint64_t timestamp;
    ClientId clientId;
    uint32_t counter;
    auto operator<=>(const LamportTimestamp&) const = default;
};
template <typename T>
struct LWWProperty {
    T value;
    LamportTimestamp lastUpdate;
    LWWProperty() : value(), lastUpdate{0, 0, 0} {}
    LWWProperty(const T& v, LamportTimestamp t) : value(v), lastUpdate(t) {}
    bool update(const T& newValue, LamportTimestamp newTimestamp) {
        if (newTimestamp > lastUpdate) {
            value = newValue;
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
        uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        uint32_t count = m_counter.fetch_add(1, std::memory_order_relaxed);
        return {now, m_clientId, count};
    }
    void merge(LamportTimestamp remote) { m_counter.store(std::max(m_counter.load(), remote.counter + 1), std::memory_order_relaxed); }
private:
    LamportClock() : m_clientId(0), m_counter(0) {}
    ClientId m_clientId;
    std::atomic<uint32_t> m_counter;
};
}
