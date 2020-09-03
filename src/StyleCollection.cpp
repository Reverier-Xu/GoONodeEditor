#include "StyleCollection.hpp"

#include <utility>

using QtNodes::ConnectionStyle;
using QtNodes::FlowViewStyle;
using QtNodes::NodeStyle;
using QtNodes::StyleCollection;

const NodeStyle &StyleCollection::nodeStyle() { return instance()._nodeStyle; }

const ConnectionStyle &StyleCollection::connectionStyle() {
    return instance()._connectionStyle;
}

const FlowViewStyle &StyleCollection::flowViewStyle() {
    return instance()._flowViewStyle;
}

void StyleCollection::setNodeStyle(NodeStyle nodeStyle) {
    instance()._nodeStyle = std::move(nodeStyle);
}

void StyleCollection::setConnectionStyle(ConnectionStyle connectionStyle) {
    instance()._connectionStyle = std::move(connectionStyle);
}

void StyleCollection::setFlowViewStyle(FlowViewStyle flowViewStyle) {
    instance()._flowViewStyle = std::move(flowViewStyle);
}

StyleCollection &StyleCollection::instance() {
    static StyleCollection collection;

    return collection;
}
