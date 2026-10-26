#pragma once

#include <QWidget>
#include <QVector>

class QPushButton;

// Left sidebar navigation for YLED.
class NavBar : public QWidget
{
    Q_OBJECT
public:
    explicit NavBar(QWidget *parent = nullptr);

    void setCurrent(int index);

signals:
    void navigate(int index);

private:
    QVector<QPushButton *> m_buttons;
};