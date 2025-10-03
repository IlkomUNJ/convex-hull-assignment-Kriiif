#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>

class DrawingCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingCanvas(QWidget *parent = nullptr);

    void resetCanvas();
    void slowConvexConvex();
    void computeConvexHull();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool flag = true;
    int stepCount = 0;
    QVector<QPoint> points;
    QVector<QPoint> upperHull;
    QVector<QPoint> lowerHull;
    QVector<QPoint> convexHull;
    QVector<QPair<QPoint, QPoint>> edgeList;
};

#endif
