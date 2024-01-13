// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "omg.h"
#include <cmath>
#include <ostream>
#include <iostream>

namespace {
const int ArrowWidth = 10;
const int ArrowHeight = 18;

QPainterPath getArrowHead(QPoint p1, QPoint p2, const int thickness)
{
    QLineF base(p1, p2);
    // Create the vector for the position of the base  of the arrowhead
    QLineF temp(QPoint(0, 0), p2 - p1);
    int val = ArrowHeight + thickness * 4;
    if (base.length() < (val - thickness * 2)) {
        val = static_cast<int>(base.length() + thickness * 2);
    }
    temp.setLength(base.length() + thickness * 2 - val);
    // Move across the line up to the head
    QPointF bottomTranslation(temp.p2());

    // Rotate base of the arrowhead
    base.setLength(ArrowWidth + thickness * 2);
    base.setAngle(base.angle() + 120);
    // Move to the correct point
    QPointF temp2 = p1 - base.p2();
    // Center it
    QPointF centerTranslation((temp2.x() / 2), (temp2.y() / 2));

    base.translate(bottomTranslation);
    base.translate(centerTranslation);

    QPainterPath path;
    path.moveTo(p1);
    path.lineTo(base.p2());
    path.lineTo(base.p1());
    path.lineTo(p2);
    return path;
}

// gets a shorter line to prevent overlap in the point of the arrow
QLine getShorterLine(QPoint p1, QPoint p2, const int thickness)
{
    QLineF l(p1, p2);
    int val = ArrowHeight + thickness * 4;
    if (l.length() < (val - thickness * 2)) {
        // here should be 0, but then we lose "angle", so this is hack, but
        // looks not very bad
        val = thickness / 4;
        l.setLength(val);
    } else {
        l.setLength(l.length() + thickness * 2 - val);
    }
    return l.toLine();
}

} // unnamed namespace

OMG::OMG(QObject* parent)
  : AbstractTwoPointTool(parent)
{
    setPadding(ArrowWidth / 2);
    m_supportsOrthogonalAdj = true;
    m_supportsDiagonalAdj = true;
}

QIcon OMG::icon(const QColor& background, bool inEditor) const
{
    Q_UNUSED(inEditor)
    return QIcon(iconPath(background) + "omg.svg");
}
QString OMG::name() const
{
    return tr("OMG");
}

CaptureTool::Type OMG::type() const
{
    return CaptureTool::TYPE_OMG;
}

QString OMG::description() const
{
    return tr("Set the OMG as the paint tool");
}


QRect OMG::boundingRect() const
{
    // if (!isValid()) {
    //     return {};
    // }

    // int offset = size() <= 1 ? 1 : static_cast<int>(round(size() / 2 + 0.5));

    // // get min and max arrow pos
    // int min_x = points().first.x();
    // int min_y = points().first.y();
    // int max_x = points().first.x();
    // int max_y = points().first.y();
    // for (int i = 0; i < m_OMGPath.elementCount(); i++) {
    //     QPointF pt = m_OMGPath.elementAt(i);
    //     if (static_cast<int>(pt.x()) < min_x) {
    //         min_x = static_cast<int>(pt.x());
    //     }
    //     if (static_cast<int>(pt.y()) < min_y) {
    //         min_y = static_cast<int>(pt.y());
    //     }
    //     if (static_cast<int>(pt.x()) > max_x) {
    //         max_x = static_cast<int>(pt.x());
    //     }
    //     if (static_cast<int>(pt.y()) > max_y) {
    //         max_y = static_cast<int>(pt.y());
    //     }
    // }

    // // get min and max line pos
    // int line_pos_min_x =
    //   std::min(std::min(points().first.x(), points().second.x()), min_x);
    // int line_pos_min_y =
    //   std::min(std::min(points().first.y(), points().second.y()), min_y);
    // int line_pos_max_x =
    //   std::max(std::max(points().first.x(), points().second.x()), max_x);
    // int line_pos_max_y =
    //   std::max(std::max(points().first.y(), points().second.y()), max_y);

    // QRect rect = QRect(line_pos_min_x - offset,
    //                    line_pos_min_y - offset,
    //                    line_pos_max_x - line_pos_min_x + offset * 2,
    //                    line_pos_max_y - line_pos_min_y + offset * 2);

    // return rect.normalized();

    return QRect(points().first, points().second).normalized();
}

CaptureTool* OMG::copy(QObject* parent)
{
    auto* tool = new OMG(parent);
    copyParams(this, tool);
    return tool;
}

void OMG::copyParams(const OMG* from, OMG* to)
{
    AbstractTwoPointTool::copyParams(from, to);
    to->m_OMGPath = this->m_OMGPath;
}

void OMG::process(QPainter& painter, const QPixmap& pixmap)
{
    // Q_UNUSED(pixmap)
    QRect selection = boundingRect().intersected(pixmap.rect());
    auto pixelRatio = pixmap.devicePixelRatio();
    QRect selectionScaled = QRect(selection.topLeft() * pixelRatio,
                                  selection.bottomRight() * pixelRatio);

    // Load the lock pick image as a QImage resource
    QImage OMGImg;
    OMGImg.load(":/img/omg.jpg");
    // std::cout << static_cast<int16_t>(OMGImg.width()) << std::endl;

    // // Compute the placement rectangle
    // int xPos = selectionScaled.center().x() - OMGImg.width() / 2;
    // int yPos = selectionScaled.center().y() - OMGImg.height() / 2;
    // QRect destRect(xPos, yPos, OMGImg.width(), OMGImg.height());

    // // Draw the lock pick image
    // painter.drawImage(destRect, OMGImg);

    // int width =
    //     static_cast<int>(selection.width() * (0.5 / qMax(1, size() + 1)));
    // int height =
    //     static_cast<int>(selection.height() * (0.5 / qMax(1, size() + 1)));
    // QSize size = QSize(qMax(width, 1), qMax(height, 1));

    // QPixmap t = QPixmap::fromImage(OMGImg);
    // t = t.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // t = t.scaled(selection.width(), selection.height());
    painter.drawImage(selection, OMGImg);
    // painter.setPen(QPen(color(), size()));
    // painter.drawLine(getShorterLine(points().first, points().second, size()));
    // m_OMGPath = getArrowHead(points().first, points().second, size());
    // painter.fillPath(m_OMGPath, QBrush(color()));
}

void OMG::pressed(CaptureContext& context)
{
    Q_UNUSED(context)
}
