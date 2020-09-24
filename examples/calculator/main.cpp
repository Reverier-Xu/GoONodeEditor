#include <QtWidgets/QApplication>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <nodes/ConnectionStyle>
#include <nodes/DataModelRegistry>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/FlowViewStyle>
#include <nodes/NodeData>
#include <nodes/NodeStyle>
#include <nodes/TypeConverter>

#include "AdditionModel.hpp"
#include "Converters.hpp"
#include "DivisionModel.hpp"
#include "ModuloModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "SubtractionModel.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::FlowViewStyle;
using QtNodes::NodeStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
    auto ret = std::make_shared<DataModelRegistry>();
    ret->registerModel<NumberSourceDataModel>("输入");

    ret->registerModel<NumberDisplayDataModel>("输出");

    ret->registerModel<AdditionModel>("运算");

    ret->registerModel<SubtractionModel>("运算");

    ret->registerModel<MultiplicationModel>("运算");

    ret->registerModel<DivisionModel>("运算");

    ret->registerModel<ModuloModel>("运算");

    ret->registerTypeConverter(
        std::make_pair(DecimalData().type(), IntegerData().type()),
        TypeConverter{DecimalToIntegerConverter()});

    ret->registerTypeConverter(
        std::make_pair(IntegerData().type(), DecimalData().type()),
        TypeConverter{IntegerToDecimalConverter()});

    return ret;
}

static void setStyle() {
    /*    FlowViewStyle::setStyle(
                R"(
      {
        "FlowViewStyle": {
          "BackgroundColor": [36, 40, 44],
          "FineGridColor": [30, 34, 38],
          "CoarseGridColor": [20, 20, 20]
        }
      }
      )");

        NodeStyle::setNodeStyle(
                R"(
      {
        "NodeStyle": {
          "NormalBoundaryColor": [30, 30, 30],
          "SelectedBoundaryColor": [0, 120, 214],
          "GradientColor0": [50, 52, 54],
          "GradientColor1": [50, 52, 54],
          "GradientColor2": [50, 52, 54],
          "GradientColor3": [50, 52, 54],
          "ShadowColor": [20, 20, 20],
          "FontColor": [240, 240, 240],
          "FontColorFaded": [180, 180, 180],
          "ConnectionPointColor": [0, 120, 214],
          "FilledConnectionPointColor": [0, 214, 120],
          "PenWidth": 0.3,
          "ErrorColor": [214, 120, 0],
          "WarningColor": [0, 60, 160],
          "HoveredPenWidth": 0.5,
          "ConnectionPointDiameter": 10.0,
          "Opacity": 1.0
        }
      }
      )");

        ConnectionStyle::setConnectionStyle(
                R"(
      {
        "ConnectionStyle": {
          "ConstructionColor": "gray",
          "NormalColor": [0, 120, 214],
          "SelectedColor": [0, 120, 214],
          "SelectedHaloColor": "deepskyblue",
          "HoveredColor": [0, 120, 255],

          "LineWidth": 3.0,
          "ConstructionLineWidth": 2.0,
          "PointDiameter": 10.0,

          "UseDataDefinedColors": false
        }
      }
      )");*/
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    setStyle();

    QWidget mainWidget;

    auto menuBar = new QToolBar();
    auto saveAction = menuBar->addAction("保存");
    auto loadAction = menuBar->addAction("加载");

    auto *l = new QVBoxLayout(&mainWidget);

    l->addWidget(menuBar);
    auto scene = new FlowScene(registerDataModels(), &mainWidget);
    l->addWidget(new FlowView(scene));
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(saveAction, &QAction::triggered, scene, &FlowScene::save);

    QObject::connect(loadAction, &QAction::triggered, scene, &FlowScene::load);

    mainWidget.setWindowTitle("GoONodeEditor");
    mainWidget.resize(800, 600);
    mainWidget.showNormal();

    return app.exec();
}
