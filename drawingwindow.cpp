#include "drawingwindow.h"
#include "drawingcanvas.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

DrawingWindow::DrawingWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Convex Hull Visualizer");
    resize(900, 600);

    QWidget *mainContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);

    canvas = new DrawingCanvas(this);

    QPushButton *btnConvex = new QPushButton("Execute Convex Hull", this);
    QPushButton *btnReset = new QPushButton("Reset Canvas", this);
    QPushButton *btnslowConvex = new QPushButton("Execute Slow Convex", this);

    QGridLayout *buttonLayout = new QGridLayout();
    buttonLayout->addWidget(btnConvex, 0, 0);
    buttonLayout->addWidget(btnReset, 0, 1);
    buttonLayout->addWidget(btnslowConvex, 0, 2);

    mainLayout->addWidget(canvas);
    mainLayout->addLayout(buttonLayout);

    setCentralWidget(mainContainer);

    connect(btnConvex, &QPushButton::clicked, canvas, &DrawingCanvas::computeConvexHull);
    connect(btnReset, &QPushButton::clicked, canvas, &DrawingCanvas::resetCanvas);
    connect(btnslowConvex, &QPushButton::clicked, canvas, &DrawingCanvas::slowConvexConvex);
}
