#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <nodes/NodeDataModel>

#include "DecimalData.hpp"
#include "MathOperationDataModel.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MultiplicationModel : public MathOperationDataModel {
   public:
    virtual ~MultiplicationModel() {}

   public:
    QString caption() const override { return QStringLiteral("乘法"); }

    QString name() const override { return QStringLiteral("乘法"); }

   private:
    void compute() override {
        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (n1 && n2) {
            modelValidationState = NodeValidationState::Valid;
            modelValidationError = QString();
            _result =
                std::make_shared<DecimalData>(n1->number() * n2->number());
        } else {
            modelValidationState = NodeValidationState::Warning;
            modelValidationError =
                QStringLiteral("输入端没有全部连接或连接不正确.");
            _result.reset();
        }

        Q_EMIT dataUpdated(outPortIndex);
    }
};
