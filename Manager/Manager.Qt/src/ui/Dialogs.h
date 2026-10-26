#pragma once

#include <QDialog>
#include <QString>

class QLabel;

/**
 * Small themed modal dialogs (replaces QMessageBox so everything stays
 * consistent with the rest of the app).
 */
class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    MessageDialog(const QString &title, const QString &message,
                  QWidget *parent = nullptr);
};

class ConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    ConfirmDialog(const QString &title, const QString &message,
                  const QString &confirmText, bool destructive,
                  QWidget *parent = nullptr);

    bool confirmed() const { return m_confirmed; }

private:
    bool m_confirmed = false;
};

namespace Dialogs {

void message(QWidget *parent, const QString &title, const QString &message);
bool confirm(QWidget *parent, const QString &title, const QString &message,
             const QString &confirmText = QStringLiteral("Delete"),
             bool destructive = true);

} // namespace Dialogs
