#pragma once
#include <vector>

namespace aui {

template <typename T>
class AAbstractListModel {
public:
    virtual ~AAbstractListModel() = default;
    virtual size_t rowCount() const = 0;
    virtual T data(size_t index) const = 0;
};

} // namespace aui
