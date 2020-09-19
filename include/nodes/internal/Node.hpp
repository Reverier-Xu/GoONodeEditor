#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "ConnectionGraphicsObject.hpp"
#include "Export.hpp"
#include "NodeData.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeState.hpp"
#include "PortType.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes {

class Connection;

class ConnectionState;

class NodeGraphicsObject;

class NodeDataModel;

class NODE_EDITOR_PUBLIC Node : public QObject, public Serializable {
    Q_OBJECT

   public:
    /// NodeDataModel should be an rvalue and is moved into the Node
    Node(std::unique_ptr<NodeDataModel> &&dataModel);

    virtual ~Node();

   public:
    QJsonObject serialize() const override;

    void unserialize(QJsonObject const &json) override;

   public:
    QUuid id() const;

    void reactToPossibleConnection(PortType, NodeDataType const &,
                                   QPointF const &scenePoint);

    void resetReactionToConnection();

   public:
    const NodeGraphicsObject &nodeGraphicsObject() const;

    NodeGraphicsObject &nodeGraphicsObject();

    void setGraphicsObject(std::unique_ptr<NodeGraphicsObject> &&graphics);

    NodeGeometry &nodeGeometry();

    const NodeGeometry &nodeGeometry() const;

    const NodeState &nodeState() const;

    NodeState &nodeState();

    NodeDataModel *nodeDataModel() const;

   public Q_SLOTS:  // data propagation

    /// 将输入的数据传输到基础的数据模型
    void transmitData(std::shared_ptr<NodeData> nodeData,
                      PortIndex inPortIndex) const;

    /// 从数据模型的输出端口拉取数据并传输到连接对象
    void onDataUpdated(PortIndex index);

    /// 当绑定的widget大小发生变化时更改node对象的大小
    void onNodeSizeUpdated();

   private:
    // uuid

    QUuid _uid;

    // data

    std::unique_ptr<NodeDataModel> _nodeDataModel;

    NodeState _nodeState;

    // painting

    NodeGeometry _nodeGeometry;

    std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
};
}  // namespace QtNodes
