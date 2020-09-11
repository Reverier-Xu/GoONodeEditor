#pragma once

#include "Connection.hpp"
#include "Node.hpp"

namespace QtNodes {

class DataModelRegistry;

class FlowScene;

class NodeDataModel;

/// 此类承担了对node对象与connection对象之间的各种操作.
/// 此类的实例应当在栈上动态创建, 并在执行完毕操作之后立刻删除.
/// 即: 不能有持久化的实例存在, 否则对节点的操作会变得超级奇怪.
class NodeConnectionInteraction {
   public:
    NodeConnectionInteraction(Node &node, Connection &connection,
                              FlowScene &scene);

    /// 节点之间能够连接的条件:
    /// 1) Connection对象 调用required()函数 '请求了' 端口
    /// 2) Connection对象的端点覆盖住了node的端口
    /// 3) Node port 是空的
    /// 4) Connection的数据类型和Port的数据类型保持一致,
    /// 或者在registry中有对应的数据转换器
    bool canConnect(PortIndex &portIndex, TypeConverter &converter) const;

    /// 1)   从canConnect()函数检查是否能够正常连接
    /// 1.5) 如果可以连接但是需要类型转换, 则在场景中添加一个转换节点,
    /// 并正确连接 2)   将node分配给Connection对象中被请求的port 3)
    /// 在NodeState中为Connection分配一个空的port 4)   调整Connection的几何显示
    /// 5)   设置模型以启动数据传输
    bool tryConnect() const;

    /// 1) Node 和 Connection 在此函数执行前应当正确连接
    /// 2) 在 NodeState 中清除 Connection 的入口点
    /// 3) 给输入端的Node传递一个INVALID类型的IN数据
    /// 4) 将Connection的端点设置为 'requiring a port'
    bool disconnect(PortType portToDisconnect) const;

   private:
    PortType connectionRequiredPort() const;

    QPointF connectionEndScenePosition(PortType) const;

    QPointF nodePortScenePosition(PortType portType, PortIndex portIndex) const;

    PortIndex nodePortIndexUnderScenePoint(PortType portType,
                                           QPointF const &p) const;

    bool nodePortIsEmpty(PortType portType, PortIndex portIndex) const;

   private:
    Node *_node;

    Connection *_connection;

    FlowScene *_scene;
};
}  // namespace QtNodes
