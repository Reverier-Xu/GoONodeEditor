#pragma once

#include <QtCore/QJsonObject>

namespace QtNodes {
/*
 * 所有可持久化存储对象的抽象父类.
 * 能够序列化的对象建议都继承自此类
 */
class Serializable {
   public:
    virtual ~Serializable() = default;

    [[nodiscard]] virtual QJsonObject serialize() const = 0;

    virtual void unserialize(QJsonObject const& /*p*/) {}
};
}  // namespace QtNodes
