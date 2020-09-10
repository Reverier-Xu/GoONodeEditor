#pragma once

#include "ConnectionStyle.hpp"
#include "FlowViewStyle.hpp"
#include "NodeStyle.hpp"

namespace QtNodes {

class StyleCollection {
   public:
    static const NodeStyle &nodeStyle();

    static const ConnectionStyle &connectionStyle();

    static const FlowViewStyle &flowViewStyle();

   public:
    static void setNodeStyle(NodeStyle);

    static void setConnectionStyle(ConnectionStyle);

    static void setFlowViewStyle(FlowViewStyle);

   private:
    StyleCollection() = default;

    StyleCollection(const StyleCollection &) = delete;

    StyleCollection &operator=(const StyleCollection &) = delete;

    static StyleCollection &instance();

   private:
    NodeStyle _nodeStyle;

    ConnectionStyle _connectionStyle;

    FlowViewStyle _flowViewStyle;
};
}  // namespace QtNodes
