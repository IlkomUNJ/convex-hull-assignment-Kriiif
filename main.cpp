#include "drawingwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DrawingWindow window;
    window.show();
    return app.exec();
}
