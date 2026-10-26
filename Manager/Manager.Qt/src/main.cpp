#include "core/DatabaseConfig.h"
#include "core/ExperimentService.h"
#include "core/Theme.h"
#include "ui/Dialogs.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QSettings>
#include <QStringList>
#include <QTimer>

namespace {

QIcon makeAppIcon()
{
    QPixmap pixmap(128, 128);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient gradient(16, 16, 112, 112);
    gradient.setColorAt(0.0, QColor(QStringLiteral("#33D6E8")));
    gradient.setColorAt(1.0, QColor(QStringLiteral("#6C6CF5")));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRect(QRect(12, 12, 104, 104));

    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(62);
    painter.setFont(font);
    painter.setPen(QColor(QStringLiteral("#04131A")));
    painter.drawText(QRect(12, 12, 104, 104), Qt::AlignCenter, QStringLiteral("Δ"));

    return QIcon(pixmap);
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Yel0w"));
    app.setApplicationName(QStringLiteral("LabManager"));
    app.setApplicationDisplayName(QStringLiteral("Lab Manager"));
    app.setWindowIcon(makeAppIcon());

    // ---- theme -------------------------------------------------------------
    QSettings settings;
    const bool dark = settings.value(QStringLiteral("theme"), QStringLiteral("dark"))
                          .toString() != QLatin1String("light");
    Theme::apply(app, dark);

    // ---- database ----------------------------------------------------------
    QString error;
    if (!DatabaseConfig::openConnection(&error)) {
        Dialogs::message(nullptr, QObject::tr("Database error"),
                         QObject::tr("LabManager.db could not be opened:\n\n%1\n\n"
                                     "Expected location:\n%2")
                             .arg(error, DatabaseConfig::databasePath()));
    } else {
        const QString root = DatabaseConfig::repositoryRoot();
        ExperimentService::importFromJsonIfEmpty(
            QDir::current().filePath(QStringLiteral("ExperimentsDB.json")));
        ExperimentService::importFromJsonIfEmpty(
            QDir(root).filePath(QStringLiteral("Manager/Manager.Windows/ExperimentsDB.json")));
        ExperimentService::importFromJsonIfEmpty(
            QDir(root).filePath(QStringLiteral("Manager/Manager.Avalonia/ExperimentsDB.json")));
    }

    // ---- window ------------------------------------------------------------
    MainWindow window;
    window.show();

    // Hidden helpers, handy for quick UI checks:
    //   --theme dark|light   force a theme
    //   --screenshot <file>  save a picture of the window then exit
    const QStringList arguments = app.arguments();
    if (const int themeIndex = arguments.indexOf(QStringLiteral("--theme"));
        themeIndex >= 0 && themeIndex + 1 < arguments.size()) {
        const bool darkTheme = arguments.at(themeIndex + 1).compare(QLatin1String("light")) != 0;
        Theme::apply(app, darkTheme);
        window.update();
    }

    if (const int shotIndex = arguments.indexOf(QStringLiteral("--screenshot"));
        shotIndex >= 0 && shotIndex + 1 < arguments.size()) {
        const QString target = arguments.at(shotIndex + 1);
        QTimer::singleShot(1500, &window, [&window, target]() {
            window.grab().save(target);
            QGuiApplication::exit(0);
        });
    }

    return app.exec();
}
