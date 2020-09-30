#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class IntegerData : public NodeData {
   public:
    IntegerData() : _number(0.0) {}

    IntegerData(int const number) : _number(number) {}

    NodeDataType type() const override {
        return NodeDataType{"integer", "整数"};
    }

    int number() const { return _number; }

    QString numberAsText() const { return QString::number(_number); }

   private:
    int _number;
};
