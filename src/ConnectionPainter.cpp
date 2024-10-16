#include "ConnectionPainter.hpp"

#include "Connection.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "StyleCollection.hpp"

using QtNodes::Connection;
using QtNodes::ConnectionGeometry;
using QtNodes::ConnectionPainter;

static QPainterPath cubicPath(ConnectionGeometry const &geom) {
    QPointF const &source = geom.source();
    QPointF const &sink = geom.sink();

    auto c1c2 = geom.pointsC1C2();

    // cubic spline
    QPainterPath cubic(source);

    cubic.cubicTo(c1c2.first, c1c2.second, sink);

    return cubic;
}

QPainterPath ConnectionPainter::getPainterStroke(
    ConnectionGeometry const &geom) {
    auto cubic = cubicPath(geom);

    QPointF const &source = geom.source();
    QPainterPath result(source);

    unsigned segments = 20;

    for (auto i = 0ul; i < segments; ++i) {
        double ratio = double(i + 1) / segments;
        result.lineTo(cubic.pointAtPercent(ratio));
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10.0);

    return stroker.createStroke(result);
}

#ifdef NODE_DEBUG_DRAWING
static void debugDrawing(QPainter *painter, Connection const &connection) {
    Q_UNUSED(painter);
    Q_UNUSED(connection);
    ConnectionGeometry const &geom = connection.connectionGeometry();

    {
        QPointF const &source = geom.source();
        QPointF const &sink = geom.sink();

        auto points = geom.pointsC1C2();

        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);

        painter->drawLine(QLineF(source, points.first));
        painter->drawLine(QLineF(points.first, points.second));
        painter->drawLine(QLineF(points.second, sink));
        painter->drawEllipse(points.first, 3, 3);
        painter->drawEllipse(points.second, 3, 3);

        painter->setBrush(Qt::NoBrush);

        painter->drawPath(cubicPath(geom));
    }

    {
        painter->setPen(Qt::yellow);

        painter->drawRect(geom.boundingRect());
    }
}
#endif

static void drawSketchLine(QPainter *painter, Connection const &connection) {
    using QtNodes::ConnectionState;

    ConnectionState const &state = connection.connectionState();

    if (state.requiresPort()) {
        auto const &connectionStyle =
            QtNodes::StyleCollection::connectionStyle();

        QPen p;
        p.setWidthF(connectionStyle.constructionLineWidth());
        p.setColor(connectionStyle.constructionColor());
        p.setStyle(Qt::DashLine);

        painter->setPen(p);
        painter->setBrush(Qt::NoBrush);

        using QtNodes::ConnectionGeometry;
        ConnectionGeometry const &geom = connection.connectionGeometry();

        auto cubic = cubicPath(geom);
        // cubic spline
        painter->drawPath(cubic);
    }
}

static void drawHoveredOrSelected(QPainter *painter,
                                  Connection const &connection) {
    using QtNodes::ConnectionGeometry;

    ConnectionGeometry const &geom = connection.connectionGeometry();
    bool const hovered = geom.hovered();

    auto const &graphicsObject = connection.getConnectionGraphicsObject();

    bool const selected = graphicsObject.isSelected();

    // drawn as a fat background
    if (hovered || selected) {
        QPen p;

        auto const &connectionStyle =
            QtNodes::StyleCollection::connectionStyle();
        double const lineWidth = connectionStyle.lineWidth();

        p.setWidthF(2 * lineWidth);
        p.setColor(selected ? connectionStyle.selectedHaloColor()
                            : connectionStyle.hoveredColor());

        painter->setPen(p);
        painter->setBrush(Qt::NoBrush);

        // cubic spline
        auto cubic = cubicPath(geom);
        painter->drawPath(cubic);
    }
}

static void drawNormalLine(QPainter *painter, Connection const &connection) {
    using QtNodes::ConnectionState;

    ConnectionState const &state = connection.connectionState();

    if (state.requiresPort()) return;

    // colors

    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    QColor normalColorOut = connectionStyle.normalColor();
    QColor normalColorIn;
    QColor selectedColor = connectionStyle.selectedColor();

    bool gradientColor = false;

    if (connectionStyle.useDataDefinedColors()) {
        using QtNodes::PortType;

        auto dataTypeOut = connection.dataType(PortType::Out);
        auto dataTypeIn = connection.dataType(PortType::In);

        gradientColor = (dataTypeOut.id != dataTypeIn.id);

        normalColorOut = QtNodes::ConnectionStyle::normalColor(dataTypeOut.id);
        normalColorIn = QtNodes::ConnectionStyle::normalColor(dataTypeIn.id);
        selectedColor = normalColorOut.lighter(120);
    }

    // geometry

    ConnectionGeometry const &geom = connection.connectionGeometry();

    double const lineWidth = connectionStyle.lineWidth();

    // draw normal line
    QPen p;

    p.setWidthF(lineWidth);

    auto const &graphicsObject = connection.getConnectionGraphicsObject();
    bool const selected = graphicsObject.isSelected();

    auto cubic = cubicPath(geom);
    if (gradientColor) {
        painter->setBrush(Qt::NoBrush);

        QColor c = normalColorOut;
        if (selected) c = c.lighter(120);
        p.setColor(c);
        painter->setPen(p);

        unsigned int const segments = 60;

        for (unsigned int i = 0ul; i < segments; ++i) {
            double ratioPrev = double(i) / segments;
            double ratio = double(i + 1) / segments;

            if (i == segments / 2) {
                QColor color_in = normalColorIn;
                if (selected) color_in = color_in.lighter(120);

                p.setColor(color_in);
                painter->setPen(p);
            }
            painter->drawLine(cubic.pointAtPercent(ratioPrev),
                              cubic.pointAtPercent(ratio));
        }

        {
            painter->setBrush(QBrush(QColor(0, 120, 214)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(cubic.pointAtPercent(0.5), 8, 8);
            painter->setBrush(Qt::NoBrush);
            QPen pen = QPen();
            pen.setWidthF(1);
            pen.setColor(QColor(100, 150, 255));
            painter->setPen(pen);
            painter->drawEllipse(cubic.pointAtPercent(0.5), 13, 13);
        }
    } else {
        p.setColor(normalColorOut);

        if (selected) {
            p.setColor(selectedColor);
        }

        painter->setPen(p);
        painter->setBrush(Qt::NoBrush);

        painter->drawPath(cubic);
    }
}

void ConnectionPainter::paint(QPainter *painter, Connection const &connection) {
    drawHoveredOrSelected(painter, connection);

    drawSketchLine(painter, connection);

    drawNormalLine(painter, connection);

#ifdef NODE_DEBUG_DRAWING
    debugDrawing(painter, connection);
#endif

    // draw end points
    ConnectionGeometry const &geom = connection.connectionGeometry();

    QPointF const &source = geom.source();
    QPointF const &sink = geom.sink();

    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    double const pointDiameter = connectionStyle.pointDiameter();
    painter->setPen(connectionStyle.constructionColor());
    painter->setBrush(connectionStyle.constructionColor());
    double const pointRadius = pointDiameter / 2.0;
    painter->drawEllipse(source, pointRadius, pointRadius);
    painter->drawEllipse(sink, pointRadius, pointRadius);
}
