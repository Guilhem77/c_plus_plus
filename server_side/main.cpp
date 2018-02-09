#include <QApplication>
#include "server.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Server window;
    window.show();

    return app.exec();
}
