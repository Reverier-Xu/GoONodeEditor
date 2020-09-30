#pragma once

#include <QtGui/QColor>

#include "Export.hpp"
#include "Style.hpp"

namespace QtNodes {

class NODE_EDITOR_PUBLIC ConnectionStyle : public Style {
   public:
    ConnectionStyle();

    explicit ConnectionStyle(QString jsonText);

   public:
    static void setConnectionStyle(QString jsonText);

   private:
    void loadJsonText(QString jsonText) override;

    void loadJsonFile(QString fileName) override;

    void loadJsonFromByteArray(QByteArray const &byteArray) override;

   public:
    [[nodiscard]] QColor constructionColor() const;

    [[nodiscard]] QColor normalColor() const;

    static QColor normalColor(const QString &typeId);

    [[nodiscard]] QColor selectedColor() const;

    [[nodiscard]] QColor selectedHaloColor() const;

    [[nodiscard]] QColor hoveredColor() const;

    [[nodiscard]] float lineWidth() const;

    [[nodiscard]] float constructionLineWidth() const;

    [[nodiscard]] float pointDiameter() const;

    [[nodiscard]] bool useDataDefinedColors() const;

   private:
    QColor ConstructionColor;
    QColor NormalColor;
    QColor SelectedColor;
    QColor SelectedHaloColor;
    QColor HoveredColor;

    float LineWidth{};
    float ConstructionLineWidth{};
    float PointDiameter{};

    bool UseDataDefinedColors{};
};
}  // namespace QtNodes
