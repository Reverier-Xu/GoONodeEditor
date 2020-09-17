#include "NodeStyle.hpp"

#include <QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>
#include <iostream>
#include <utility>

#include "StyleCollection.hpp"

using QtNodes::NodeStyle;

inline void initResources() { Q_INIT_RESOURCE(resources); }

NodeStyle::NodeStyle() {
    // 显式初始化静态资源文件, 免得自动初始化搞出什么幺蛾子
    // 自动初始化的fiasco错误在这里提到过:
    // https://isocpp.org/wiki/faq/ctors#static-init-order 这啥语言啊...一堆坑
    initResources();

    // 此配置文件使用qrc直接写死到程序里面, 作为默认主题使用.
    // 用户需要其他的主题可以用setStyle覆盖掉此项
    loadJsonFile(":DefaultStyle.json");
}

NodeStyle::NodeStyle(QString jsonText) { loadJsonText(std::move(jsonText)); }

void NodeStyle::setNodeStyle(QString jsonText) {
    NodeStyle style(std::move(jsonText));

    StyleCollection::setNodeStyle(style);
}

#ifdef STYLE_DEBUG
#define NODE_STYLE_CHECK_UNDEFINED_VALUE(v, variable)                          \
    {                                                                          \
        if (v.type() == QJsonValue::Undefined || v.type() == QJsonValue::Null) \
            qWarning() << "Undefined value for parameter:" << #variable;       \
    }
#else
#define NODE_STYLE_CHECK_UNDEFINED_VALUE(v, variable)
#endif

#define NODE_STYLE_READ_COLOR(values, variable)                                \
    {                                                                          \
        auto valueRef = values[#variable];                                     \
        NODE_STYLE_CHECK_UNDEFINED_VALUE(valueRef, variable)                   \
        if (valueRef.isArray()) {                                              \
            auto colorArray = valueRef.toArray();                              \
            std::vector<int> rgb;                                              \
            rgb.reserve(3);                                                    \
            for (auto it = colorArray.begin(); it != colorArray.end(); ++it) { \
                rgb.push_back((*it).toInt());                                  \
            }                                                                  \
            variable = QColor(rgb[0], rgb[1], rgb[2]);                         \
        } else {                                                               \
            variable = QColor(valueRef.toString());                            \
        }                                                                      \
    }

#define NODE_STYLE_READ_FLOAT(values, variable)              \
    {                                                        \
        auto valueRef = values[#variable];                   \
        NODE_STYLE_CHECK_UNDEFINED_VALUE(valueRef, variable) \
        variable = valueRef.toDouble();                      \
    }

void NodeStyle::loadJsonFile(QString styleFile) {
    QFile file(styleFile);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file " << styleFile;

        return;
    }

    loadJsonFromByteArray(file.readAll());
}

void NodeStyle::loadJsonText(QString jsonText) {
    loadJsonFromByteArray(jsonText.toUtf8());
}

void NodeStyle::loadJsonFromByteArray(QByteArray const &byteArray) {
    QJsonDocument json(QJsonDocument::fromJson(byteArray));

    QJsonObject topLevelObject = json.object();

    QJsonValueRef nodeStyleValues = topLevelObject["NodeStyle"];

    QJsonObject obj = nodeStyleValues.toObject();

    NODE_STYLE_READ_COLOR(obj, NormalBoundaryColor);
    NODE_STYLE_READ_COLOR(obj, SelectedBoundaryColor);
    NODE_STYLE_READ_COLOR(obj, GradientColor0);
    NODE_STYLE_READ_COLOR(obj, GradientColor1);
    NODE_STYLE_READ_COLOR(obj, GradientColor2);
    NODE_STYLE_READ_COLOR(obj, GradientColor3);
    NODE_STYLE_READ_COLOR(obj, ShadowColor);
    NODE_STYLE_READ_COLOR(obj, FontColor);
    NODE_STYLE_READ_COLOR(obj, FontColorFaded);
    NODE_STYLE_READ_COLOR(obj, ConnectionPointColor);
    NODE_STYLE_READ_COLOR(obj, FilledConnectionPointColor);
    NODE_STYLE_READ_COLOR(obj, WarningColor);
    NODE_STYLE_READ_COLOR(obj, ErrorColor);

    NODE_STYLE_READ_FLOAT(obj, PenWidth);
    NODE_STYLE_READ_FLOAT(obj, HoveredPenWidth);
    NODE_STYLE_READ_FLOAT(obj, ConnectionPointDiameter);

    NODE_STYLE_READ_FLOAT(obj, Opacity);
}
