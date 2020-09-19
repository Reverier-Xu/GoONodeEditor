#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class QGraphicsSceneMouseEvent;

namespace QtNodes {

class FlowScene;

class Connection;

class ConnectionGeometry;

class Node;

/// connection 的 Graphic Object. 将自己添加到scene之中
class ConnectionGraphicsObject : public QGraphicsObject {
    Q_OBJECT

   public:
    ConnectionGraphicsObject(FlowScene &scene, Connection &connection);

    ~ConnectionGraphicsObject() override;

    enum { Type = UserType + 2 };

    int type() const override { return Type; }

   public:
    Connection &connection();

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void setGeometryChanged();

    /// 更新连接对象两个端点的位置
    void move();

    void lock(bool locked);

   protected:
    void paint(QPainter *painter, QStyleOptionGraphicsItem const *option,
               QWidget *widget = 0) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

   private:
    void addGraphicsEffect();

   private:
    FlowScene &_scene;

    Connection &_connection;
};
}  // namespace QtNodes
