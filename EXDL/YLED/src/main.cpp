#include <QApplication>
#include <QFile>
#include <QFont>
#include <QStyleFactory>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("YLED"));
    QApplication::setOrganizationName(QStringLiteral("Yel0w"));
    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // A consistent modern base font across platforms.
    QFont font = app.font();
    font.setPointSizeF(10.0);
    app.setFont(font);

    QFile qss(QStringLiteral(":/style.qss"));
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(qss.readAll()));

    MainWindow window;
    window.show();

    return app.exec();
}