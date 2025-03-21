#include "pch.h"
#include "QVpj.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QVpj w;
    w.show();
    return a.exec();
}
