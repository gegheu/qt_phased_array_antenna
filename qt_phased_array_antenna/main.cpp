#include "communication_ui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    communication window;
    window.show();
    return app.exec();
}
