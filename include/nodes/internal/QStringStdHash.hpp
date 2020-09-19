#pragma once

#include <QtGlobal>

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))

// Qt 5.14 实现了一个专门的 std::hash<QString>, 就不需要这玩意儿了

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <functional>

namespace std {
template <>
struct hash<QString> {
    inline std::size_t operator()(QString const &s) const { return qHash(s); }
};
}  // namespace std

#endif
