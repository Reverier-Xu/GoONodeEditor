#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"
#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes {

class FlowScene;

class FlowItemEntry;

/// 该类对GUI事件，鼠标单击做出反应，并转发绘画操作。
class NodeGraphicsObject : public QGraphicsObject {
    Q_OBJECT

   public:
    NodeGraphicsObject(FlowScene &scene, Node &node);
    ~NodeGraphicsObject() override;

    Node &node();

    const Node &node() const;

    QRectF boundingRect() const override;

    void setGeometryChanged();

    /// 访问所有连接的连接并更正其相应的端点。
    void moveConnections() const;

    enum { Type = UserType + 1 };

    int type() const override { return Type; }

    void lock(bool locked);

   protected:
    void paint(QPainter *painter, QStyleOptionGraphicsItem const *option,
               QWidget *widget) override;

    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

   private:
    void embedQWidget();

   private:
    FlowScene &_scene;

    Node &_node;

    bool _locked;

    // 可以是nullptr或由父QGraphicsItem拥有
    QGraphicsProxyWidget *_proxyWidget;
};
}  // namespace QtNodes
