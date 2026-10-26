#include "ui/Dialogs.h"

#include "core/Theme.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

QWidget *buildBody(const QString &title, const QString &message, const QString &objectName)
{
    Q_UNUSED(objectName)

    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(24, 22, 24, 8);
    layout->setSpacing(10);

    auto *titleLabel = new QLabel(title);
    QFont titleFont = Theme::scaledFont(titleLabel->font(), 6);
    titleFont.setWeight(QFont::DemiBold);
    titleLabel->setFont(titleFont);

    auto *messageLabel = new QLabel(message);
    messageLabel->setObjectName(QStringLiteral("muted"));
    messageLabel->setWordWrap(true);
    messageLabel->setMinimumWidth(340);
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    return container;
}

QPushButton *makeButton(const QString &text, const char *objectName)
{
    auto *button = new QPushButton(text);
    button->setObjectName(QString::fromLatin1(objectName));
    button->setMinimumWidth(96);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

} // namespace

MessageDialog::MessageDialog(const QString &title, const QString &message, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    outer->addWidget(buildBody(title, message, "message"));

    auto *buttons = new QDialogButtonBox;
    buttons->setCenterButtons(false);

    QPushButton *ok = makeButton(tr("OK"), "primaryButton");
    buttons->addButton(ok, QDialogButtonBox::AcceptRole);
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->setContentsMargins(18, 4, 18, 18);
    buttonRow->addStretch();
    buttonRow->addWidget(buttons);
    outer->addLayout(buttonRow);
}

ConfirmDialog::ConfirmDialog(const QString &title, const QString &message,
                             const QString &confirmText, bool destructive, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    outer->addWidget(buildBody(title, message, "confirm"));

    auto *buttons = new QDialogButtonBox;

    QPushButton *cancel = makeButton(tr("Cancel"), "softButton");
    QPushButton *confirm = makeButton(confirmText, destructive ? "dangerButton" : "primaryButton");

    buttons->addButton(cancel, QDialogButtonBox::RejectRole);
    buttons->addButton(confirm, QDialogButtonBox::AcceptRole);

    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(confirm, &QPushButton::clicked, this, [this]() {
        m_confirmed = true;
        accept();
    });

    auto *buttonRow = new QHBoxLayout;
    buttonRow->setContentsMargins(18, 4, 18, 18);
    buttonRow->addStretch();
    buttonRow->addWidget(buttons);
    outer->addLayout(buttonRow);
}

namespace Dialogs {

void message(QWidget *parent, const QString &title, const QString &message)
{
    MessageDialog dialog(title, message, parent);
    dialog.exec();
}

bool confirm(QWidget *parent, const QString &title, const QString &message,
             const QString &confirmText, bool destructive)
{
    ConfirmDialog dialog(title, message, confirmText, destructive, parent);
    return dialog.exec() == QDialog::Accepted && dialog.confirmed();
}

} // namespace Dialogs
