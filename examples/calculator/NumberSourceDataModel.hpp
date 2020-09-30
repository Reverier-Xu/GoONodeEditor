#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
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
class NumberSourceDataModel : public NodeDataModel {
    Q_OBJECT

   public:
    NumberSourceDataModel();

    virtual ~NumberSourceDataModel() {}

   public:
    QString caption() const override { return QStringLiteral("输入"); }

    bool captionVisible() const override { return false; }

    QString name() const override { return QStringLiteral("输入"); }

   public:
    QJsonObject serialize() const override;

    void unserialize(QJsonObject const &p) override;

   public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType,
                          PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData>, int) override {}

    QWidget *embeddedWidget() override { return _lineEdit; }

   private Q_SLOTS:

    void onTextEdited(QString const &string);

   private:
    std::shared_ptr<DecimalData> _number;

    QLineEdit *_lineEdit;
};
