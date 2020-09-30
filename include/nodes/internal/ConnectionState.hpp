#pragma once

#include <QtCore/QUuid>

#include "PortType.hpp"

class QPointF;

namespace QtNodes {

class Node;

/// Stores currently draggind end.
/// Remembers last hovered Node.
class ConnectionState {
   public:
    explicit ConnectionState(PortType port = PortType::None)
        : _requiredPort(port) {}

    ConnectionState(const ConnectionState &) = delete;

    ConnectionState operator=(const ConnectionState &) = delete;

    ~ConnectionState();

   public:
    void setRequiredPort(PortType end) { _requiredPort = end; }

    [[nodiscard]] PortType requiredPort() const { return _requiredPort; }

    [[nodiscard]] bool requiresPort() const {
        return _requiredPort != PortType::None;
    }

    void setNoRequiredPort() { _requiredPort = PortType::None; }

   public:
    void interactWithNode(Node *node);

    void setLastHoveredNode(Node *node);

    [[nodiscard]] Node *lastHoveredNode() const { return _lastHoveredNode; }

    void resetLastHoveredNode();

   private:
    PortType _requiredPort;

    Node *_lastHoveredNode{nullptr};
};
}  // namespace QtNodes
