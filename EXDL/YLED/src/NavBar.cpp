#include "NavBar.h"

#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>

NavBar::NavBar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("navBar"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(236);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(18, 22, 18, 16);
    root->setSpacing(0);

    auto *brand = new QLabel(QStringLiteral("YLED"), this);
    brand->setObjectName(QStringLiteral("brand"));

    auto *sub = new QLabel(QStringLiteral("Yel0w's Lab\nExperiment Donloader"), this);
    sub->setObjectName(QStringLiteral("brandSub"));
    sub->setWordWrap(true);

    root->addWidget(brand);
    root->addSpacing(2);
    root->addWidget(sub);
    root->addSpacing(26);

    auto *group = new QButtonGroup(this);
    group->setExclusive(true);

    const int pages[] = { 0, 2 };
    const char *labels[] = { "Experiments", "Settings" };

    for (int i = 0; i < 2; ++i) {
        auto *btn = new QPushButton(QString::fromLatin1(labels[i]), this);
        btn->setObjectName(QStringLiteral("navBtn"));
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("pageId", pages[i]);
        group->addButton(btn, pages[i]);
        m_buttons << btn;
        root->addWidget(btn);
        root->addSpacing(4);
    }

    root->addStretch(1);

    auto *footer = new QLabel(QStringLiteral("The Lab"), this);
    footer->setObjectName(QStringLiteral("navFooter"));

    auto *link = new QPushButton(QStringLiteral("github.com/Yel0w08/Lab"), this);
    link->setObjectName(QStringLiteral("link"));
    link->setCursor(Qt::PointingHandCursor);
    connect(link, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/Yel0w08/Lab")));
    });

    root->addWidget(footer);
    root->addWidget(link);

    connect(group, &QButtonGroup::idClicked, this, [this](int id) {
        emit navigate(id);
    });

    m_buttons.first()->setChecked(true);
}

void NavBar::setCurrent(int index)
{
    for (QPushButton *b : m_buttons) {
        const int page = b->property("pageId").toInt();
        // Detail page (1) belongs to the Experiments section (0).
        b->setChecked(page == index || (page == 0 && index == 1));
    }
}
