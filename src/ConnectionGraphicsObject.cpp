#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include "Connection.hpp"
#include "ConnectionBlurEffect.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "Node.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeGraphicsObject.hpp"

using QtNodes::Connection;
using QtNodes::ConnectionGraphicsObject;
using QtNodes::FlowScene;

ConnectionGraphicsObject::ConnectionGraphicsObject(FlowScene &scene,
                                                   Connection &connection)
    : _scene(scene), _connection(connection) {
    _scene.addItem(this);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setAcceptHoverEvents(true);

    // addGraphicsEffect();

    setZValue(-1.0);
}

ConnectionGraphicsObject::~ConnectionGraphicsObject() {
    _scene.removeItem(this);
}

QtNodes::Connection &ConnectionGraphicsObject::connection() {
    return _connection;
}

QRectF ConnectionGraphicsObject::boundingRect() const {
    return _connection.connectionGeometry().boundingRect();
}

QPainterPath ConnectionGraphicsObject::shape() const {
#ifdef DEBUG_DRAWING

    // QPainterPath path;

    // path.addRect(boundingRect());
    // return path;

#else
    auto const &geom = _connection.connectionGeometry();

    return ConnectionPainter::getPainterStroke(geom);

#endif
}

void ConnectionGraphicsObject::setGeometryChanged() { prepareGeometryChange(); }

void ConnectionGraphicsObject::move() {
    /* Qt文档: 如果需要从Rect对象获取相对位移, 那么就需要inverted()
     * scene pos: 对象在整个scene中的绝对坐标
     * connection pos: 对象相对于自身的偏移.
     * 相对偏移 = transform.map(绝对位移)
     * 绝对位移 = transform.inverted().map(相对位移)
     */

    /* 由于连接是从牵出位置定义的,
     * 所以在重定位之后整个对象的位置位于牵出节点的Port处.
     * 此时需要计算两个端点的相对位置, 其中一个端点就位于牵出的Port处,
     * 相对位置是(0,0) 另一个端点的位置就需要结合另一个node计算出来,
     * 然后才能够正常进行绘制.
     * Connection的绘制方法是作为一个GraphicsItem整体来绘制的, 使用QPainter
     * 所以坐标需要相对于对象来绘制而不是相对于整个scene.
     * 整个item已经在scene中定位过了
     */

    for (PortType portType : {PortType::In, PortType::Out}) {
        if (auto node = _connection.getNode(portType)) {
            auto const &nodeGraphics = node->nodeGraphicsObject();

            auto const &nodeGeom = node->nodeGeometry();  // 获取节点的几何对象

            QPointF
                scenePos =  // 调用node几何对象来查询对应端口相对于整个scene的几何位置
                nodeGeom.portScenePosition(_connection.getPortIndex(portType),
                                           portType,
                                           nodeGraphics.sceneTransform());
            // qDebug() << "connection scene pos: " << scenePos;

            QTransform sceneTransform =
                this->sceneTransform();  // 连接的图形转换控制对象

            QPointF connectionPos = sceneTransform.inverted().map(scenePos);
            // qDebug() << "connection relative pos: " << connectionPos;

            _connection.connectionGeometry().setEndPoint(portType,
                                                         connectionPos);

            _connection.getConnectionGraphicsObject().setGeometryChanged();
            _connection.getConnectionGraphicsObject().update();
        }
    }
}

void ConnectionGraphicsObject::lock(bool locked) {
    // 锁
    setFlag(QGraphicsItem::ItemIsMovable, !locked);
    setFlag(QGraphicsItem::ItemIsFocusable, !locked);
    setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}

void ConnectionGraphicsObject::paint(QPainter *painter,
                                     QStyleOptionGraphicsItem const *option,
                                     QWidget *) {
    painter->setClipRect(option->exposedRect);

    ConnectionPainter::paint(painter, _connection);
}

void ConnectionGraphicsObject::mousePressEvent(
    QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
    // event->ignore();
}

void ConnectionGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();

    auto view = static_cast<QGraphicsView *>(event->widget());
    auto node = locateNodeAt(event->scenePos(), _scene, view->transform());

    auto &state = _connection.connectionState();

    state.interactWithNode(node);
    if (node) {
        node->reactToPossibleConnection(
            state.requiredPort(),
            _connection.dataType(oppositePort(state.requiredPort())),
            event->scenePos());
    }

    //-------------------

    QPointF offset = event->pos() - event->lastPos();

    auto requiredPort = _connection.requiredPort();

    if (requiredPort != PortType::None) {
        _connection.connectionGeometry().moveEndPoint(requiredPort, offset);
    }

    //-------------------

    update();

    event->accept();
}

void ConnectionGraphicsObject::mouseReleaseEvent(
    QGraphicsSceneMouseEvent *event) {
    ungrabMouse();
    event->accept();

    auto node =
        locateNodeAt(event->scenePos(), _scene, _scene.views()[0]->transform());

    NodeConnectionInteraction interaction(*node, _connection, _scene);

    if (node && interaction.tryConnect()) {
        node->resetReactionToConnection();
    }

    if (_connection.connectionState().requiresPort()) {
        _scene.deleteConnection(_connection);
    }
}

void ConnectionGraphicsObject::hoverEnterEvent(
    QGraphicsSceneHoverEvent *event) {
    _connection.connectionGeometry().setHovered(true);

    update();
    _scene.connectionHovered(connection(), event->screenPos());
    event->accept();
}

void ConnectionGraphicsObject::hoverLeaveEvent(
    QGraphicsSceneHoverEvent *event) {
    _connection.connectionGeometry().setHovered(false);

    update();
    _scene.connectionHoverLeft(connection());
    event->accept();
}

void ConnectionGraphicsObject::addGraphicsEffect() {
    auto effect = new QGraphicsBlurEffect;

    effect->setBlurRadius(5);
    setGraphicsEffect(effect);

    // auto effect = new QGraphicsDropShadowEffect;
    // auto effect = new ConnectionBlurEffect(this);
    // effect->setOffset(4, 4);
    // effect->setColor(QColor(Qt::gray).darker(800));
}
