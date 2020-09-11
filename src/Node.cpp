#include "Node.hpp"

#include <utility>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "NodeDataModel.hpp"
#include "NodeGraphicsObject.hpp"

using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeState;
using QtNodes::PortIndex;
using QtNodes::PortType;

Node::Node(std::unique_ptr<NodeDataModel> &&dataModel)
    : _uid(QUuid::createUuid()),
      _nodeDataModel(std::move(dataModel)),
      _nodeState(_nodeDataModel),
      _nodeGeometry(_nodeDataModel),
      _nodeGraphicsObject(nullptr) {
    _nodeGeometry.recalculateSize();

    // transmit data: model => node
    connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated, this,
            &Node::onDataUpdated);

    connect(_nodeDataModel.get(), &NodeDataModel::embeddedWidgetSizeUpdated,
            this, &Node::onNodeSizeUpdated);
}

Node::~Node() = default;

QJsonObject Node::serialize() const {
    QJsonObject nodeJson;

    nodeJson["id"] = _uid.toString();

    nodeJson["model"] = _nodeDataModel->serialize();

    QJsonObject obj;
    obj["x"] = _nodeGraphicsObject->pos().x();
    obj["y"] = _nodeGraphicsObject->pos().y();
    nodeJson["position"] = obj;

    return nodeJson;
}

void Node::unserialize(QJsonObject const &json) {
    _uid = QUuid(json["id"].toString());

    QJsonObject positionJson = json["position"].toObject();
    QPointF point(positionJson["x"].toDouble(), positionJson["y"].toDouble());
    _nodeGraphicsObject->setPos(point);

    _nodeDataModel->unserialize(json["model"].toObject());
}

QUuid Node::id() const { return _uid; }

void Node::reactToPossibleConnection(PortType reactingPortType,
                                     NodeDataType const &reactingDataType,
                                     QPointF const &scenePoint) {
    QTransform const t = _nodeGraphicsObject->sceneTransform();

    QPointF p = t.inverted().map(scenePoint);

    _nodeGeometry.setDraggingPosition(p);

    _nodeGraphicsObject->update();

    _nodeState.setReaction(NodeState::REACTING, reactingPortType,
                           reactingDataType);
}

void Node::resetReactionToConnection() {
    _nodeState.setReaction(NodeState::NOT_REACTING);
    _nodeGraphicsObject->update();
}

const NodeGraphicsObject &Node::nodeGraphicsObject() const {
    return *_nodeGraphicsObject;
}

NodeGraphicsObject &Node::nodeGraphicsObject() { return *_nodeGraphicsObject; }

void Node::setGraphicsObject(std::unique_ptr<NodeGraphicsObject> &&graphics) {
    _nodeGraphicsObject = std::move(graphics);

    _nodeGeometry.recalculateSize();
}

NodeGeometry &Node::nodeGeometry() { return _nodeGeometry; }

const NodeGeometry &Node::nodeGeometry() const { return _nodeGeometry; }

NodeState const &Node::nodeState() const { return _nodeState; }

NodeState &Node::nodeState() { return _nodeState; }

NodeDataModel *Node::nodeDataModel() const { return _nodeDataModel.get(); }

void Node::transmitData(std::shared_ptr<NodeData> nodeData,
                        PortIndex inPortIndex) const {
    _nodeDataModel->setInData(std::move(nodeData), inPortIndex);

    // 重新计算节点的视觉效果.
    // 数据更改可能导致节点占用比以前更多的空间,
    // 因为这会在受影响的节点上强制进行重新计算 + 重新绘制.
    // TODO: 想办法修一下这里的内存泄露 (修不了就算了, 谁会差那几十KB内存啊)
    _nodeGraphicsObject->setGeometryChanged();
    _nodeGeometry.recalculateSize();
    _nodeGraphicsObject->update();
    _nodeGraphicsObject->moveConnections();
}

void Node::onDataUpdated(PortIndex index) {
    auto nodeData = _nodeDataModel->outData(index);

    auto connections = _nodeState.connections(PortType::Out, index);

    for (auto const &c : connections) c.second->transmitData(nodeData);
}

void Node::onNodeSizeUpdated() {
    if (nodeDataModel()->embeddedWidget()) {
        nodeDataModel()->embeddedWidget()->adjustSize();
    }
    nodeGeometry().recalculateSize();
    for (PortType type : {PortType::In, PortType::Out}) {
        for (auto &conn_set : nodeState().getEntries(type)) {
            for (auto &pair : conn_set) {
                Connection *conn = pair.second;
                conn->getConnectionGraphicsObject().move();
            }
        }
    }
}
