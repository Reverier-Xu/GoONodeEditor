#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <iostream>
#include <nodes/NodeDataModel>

class DecimalData;

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MathOperationDataModel : public NodeDataModel {
    Q_OBJECT

   public:
    ~MathOperationDataModel() override = default;

   public:
    [[nodiscard]] unsigned int nPorts(PortType portType) const override;

    [[nodiscard]] NodeDataType dataType(PortType portType,
                                        PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data,
                   PortIndex portIndex) override;

    QWidget* embeddedWidget() override { return nullptr; }

    [[nodiscard]] NodeValidationState validationState() const override;

    [[nodiscard]] QString validationMessage() const override;

   protected:
    virtual void compute() = 0;

   protected:
    std::weak_ptr<DecimalData> _number1;
    std::weak_ptr<DecimalData> _number2;

    std::shared_ptr<DecimalData> _result;

    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = QString("输入端没有全部连接或连接不正确.");
};
