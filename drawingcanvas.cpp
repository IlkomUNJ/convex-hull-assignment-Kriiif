#include "drawingcanvas.h"

DrawingCanvas::DrawingCanvas(QWidget *parent)
    : QWidget{parent}
{}

void DrawingCanvas::resetCanvas()
{
    points.clear();
    upperHull.clear();
    lowerHull.clear();
    convexHull.clear();
    edgeList.clear();
    stepCount = 0;
    update();
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        points.append(event->pos());
        convexHull.clear();
        edgeList.clear();
        stepCount = 0;
        update();
    }
}

void DrawingCanvas::slowConvexConvex()
{
    edgeList.clear();
    convexHull.clear();
    stepCount = 0;

    if (points.size() < 3) {
        convexHull = points;
        update();
        return;
    }

    QVector<QPoint> hullPoints;

    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < points.size(); j++) {
            if (i == j) continue;
            QPoint p = points[i];
            QPoint q = points[j];

            bool isEdge = true;
            for (int k = 0; k < points.size(); ++k) {
                stepCount++;
                if (k == i || k == j) continue;
                QPoint r = points[k];

                if (((q.x() - p.x()) * (r.y() - p.y()) -
                     (q.y() - p.y()) * (r.x() - p.x())) > 0) {
                    isEdge = false;
                    break;
                }
            }
            if (isEdge) {
                // simpan titik ujung ke kandidat hull
                if (!hullPoints.contains(p)) hullPoints.append(p);
                if (!hullPoints.contains(q)) hullPoints.append(q);
            }
        }
    }

    QPoint anchor = *std::min_element(hullPoints.begin(), hullPoints.end(),
                                      [](const QPoint &a, const QPoint &b) {
                                          return (a.x() < b.x()) || (a.x() == b.x() && a.y() < b.y());
                                      });

    // fungsi cross product
    auto cross = [](const QPoint &a, const QPoint &b, const QPoint &c) {
        return (b.x() - a.x()) * (c.y() - a.y()) -
               (b.y() - a.y()) * (c.x() - a.x());
    };

    // urutkan berdasarkan sudut polar relatif ke anchor
    std::sort(hullPoints.begin(), hullPoints.end(),
              [anchor](const QPoint &a, const QPoint &b) {
                  int cp = (a.x() - anchor.x()) * (b.y() - anchor.y()) -
                           (a.y() - anchor.y()) * (b.x() - anchor.x());
                  if (cp == 0) {
                      int da = (a.x() - anchor.x()) * (a.x() - anchor.x()) +
                               (a.y() - anchor.y()) * (a.y() - anchor.y());
                      int db = (b.x() - anchor.x()) * (b.x() - anchor.x()) +
                               (b.y() - anchor.y()) * (b.y() - anchor.y());
                      return da < db;
                  }
                  return cp > 0;
              });

    convexHull = hullPoints;

    update();
}

void DrawingCanvas::computeConvexHull()
{
    upperHull.clear();
    lowerHull.clear();
    edgeList.clear();
    convexHull.clear();
    stepCount = 0;

    if (points.size() < 2) {
        return;
    }

    std::sort(points.begin(), points.end(), [](const QPoint &p1, const QPoint &p2) {
        if (p1.x() == p2.x())
            return p1.y() < p2.y();
        return p1.x() < p2.x();
    });

    auto crossProduct = [](const QPoint &p1, const QPoint &p2, const QPoint &p3) {
        return (p2.x() - p1.x()) * (p3.y() - p1.y()) -
               (p2.y() - p1.y()) * (p3.x() - p1.x());
    };

    upperHull.reserve(points.size());
    upperHull << points[0] << points[1];

    for (int i = 2; i < points.size(); ++i) {
        upperHull.append(points[i]);
        while (upperHull.size() >= 3) {
            int n = upperHull.size();
            stepCount++;
            if (crossProduct(upperHull[n - 3], upperHull[n - 2], upperHull[n - 1]) <= 0) {
                upperHull.removeAt(n - 2);
            } else {
                break;
            }
        }
    }

    lowerHull.reserve(points.size());
    lowerHull << points.last() << points[points.size() - 2];

    for (int i = points.size() - 3; i >= 0; --i) {
        lowerHull.append(points[i]);
        while (lowerHull.size() >= 3) {
            stepCount++;
            int n = lowerHull.size();
            if (crossProduct(lowerHull[n - 3], lowerHull[n - 2], lowerHull[n - 1]) <= 0) {
                lowerHull.removeAt(n - 2);
            } else {
                break;
            }
        }
    }

    convexHull = upperHull;
    for (int i = 1; i + 1 < lowerHull.size(); ++i) {
        convexHull.append(lowerHull[i]);
    }

    update();
}

void DrawingCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);

    painter.drawText(10, 20, "Total points: " + QString::number(points.size()));
    painter.drawText(10, 40, "Step count: " + QString::number(stepCount));
    painter.setRenderHint(QPainter::Antialiasing);

    if (!convexHull.isEmpty()) {
        QPen hullPen(Qt::blue, 5, Qt::SolidLine);
        painter.setPen(hullPen);
        for (int i = 0; i < convexHull.size(); i++) {
            painter.drawLine(convexHull[i], convexHull[(i + 1) % convexHull.size()]);
        }
    }

    if (!edgeList.isEmpty()) {
        QPen edgePen(Qt::darkYellow, 2, Qt::SolidLine);
        painter.setPen(edgePen);
        for (const auto &edge : edgeList) {
            painter.drawLine(edge.first, edge.second);
        }
    }

    if (!upperHull.isEmpty()) {
        QPen upperPen(Qt::magenta, 3, Qt::SolidLine);
        painter.setPen(upperPen);
        for (int i = 0; i < upperHull.size() - 1; ++i) {
            painter.drawLine(upperHull[i], upperHull[i+1]);
        }
    }
    if (!lowerHull.isEmpty()) {
        QPen lowerPen(Qt::green, 3, Qt::SolidLine);
        painter.setPen(lowerPen);
        for (int i = 0; i < lowerHull.size() - 1; ++i) {
            painter.drawLine(lowerHull[i], lowerHull[i+1]);
        }
    }

    // Draw the individual points in red
    QPen dotPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(dotPen);
    for (const QPoint &pt : points) {
        painter.drawPoint(pt);
    }
}
