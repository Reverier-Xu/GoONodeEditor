#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include "ConnectionGeometry.hpp"
#include "ConnectionState.hpp"
#include "Export.hpp"
#include "NodeData.hpp"
#include "PortType.hpp"
#include "QUuidStdHash.hpp"
#include "Serializable.hpp"
#include "TypeConverter.hpp"
#include "memory.hpp"

class QPointF;

namespace QtNodes {

class Node;

class NodeData;

class ConnectionGraphicsObject;

///
class NODE_EDITOR_PUBLIC Connection : public QObject, public Serializable {
    Q_OBJECT

   public:
    /// New Connection is attached to the port of the given Node.
    /// The port has parameters (portType, portIndex).
    /// The opposite connection end will require anothre port.
    Connection(PortType portType, Node &node, PortIndex portIndex);

    Connection(Node &nodeIn, PortIndex portIndexIn, Node &nodeOut,
               PortIndex portIndexOut,
               TypeConverter converter = TypeConverter{});

    Connection(const Connection &) = delete;

    Connection operator=(const Connection &) = delete;

    ~Connection() override;

   public:
    QJsonObject serialize() const override;

   public:
    QUuid id() const;

    /// Remembers the end being dragged.
    /// Invalidates Node address.
    /// Grabs mouse.
    void setRequiredPort(PortType portType);

    PortType requiredPort() const;

    void setGraphicsObject(
        std::unique_ptr<ConnectionGraphicsObject> &&graphics);

    /// 将节点分配到对应的端口
    /// 本函数使用时要确保对应的端口一定存在, 没有对合法性进行检查
    void setNodeToPort(Node &node, PortType portType, PortIndex portIndex);

    void removeFromNodes() const;

   public:
    ConnectionGraphicsObject &getConnectionGraphicsObject() const;

    ConnectionState const &connectionState() const;

    ConnectionState &connectionState();

    ConnectionGeometry &connectionGeometry();

    ConnectionGeometry const &connectionGeometry() const;

    Node *getNode(PortType portType) const;

    Node *&getNode(PortType portType);

    PortIndex getPortIndex(PortType portType) const;

    void clearNode(PortType portType);

    NodeDataType dataType(PortType portType) const;

    void setTypeConverter(TypeConverter converter);

    bool complete() const;

   public:  // data propagation
    void transmitData(std::shared_ptr<NodeData> nodeData) const;

    void transmitEmptyData() const;

   Q_SIGNALS:

    void connectionCompleted(Connection const &) const;

    void connectionMadeIncomplete(Connection const &) const;

   private:
    QUuid _uid;

   private:
    Node *_outNode = nullptr;
    Node *_inNode = nullptr;

    PortIndex _outPortIndex;
    PortIndex _inPortIndex;

   private:
    ConnectionState _connectionState;
    ConnectionGeometry _connectionGeometry;

    std::unique_ptr<ConnectionGraphicsObject> _connectionGraphicsObject;

    TypeConverter _converter;

   Q_SIGNALS:

    void updated(Connection &conn) const;
};
}  // namespace QtNodes
