#pragma once

#include <QStyledItemDelegate>

class ExperimentTableModel;

/**
 * Row painting shared by every column: selection / hover background,
 * row separator, then column specific content.
 */
class BaseDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BaseDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

protected:
    virtual void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

    void drawPlain(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    void drawText(QPainter *painter, const QStyleOptionViewItem &option,
                  const QRect &rect, const QString &text, const QFont &font,
                  const QColor &color) const;

    const ExperimentTableModel *m_model;
};

/** ★ / ☆ column. */
class StarDelegate : public BaseDelegate
{
    Q_OBJECT
public:
    explicit StarDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

protected:
    void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const override;
};

/** Rounded status pill. */
class StatusDelegate : public BaseDelegate
{
    Q_OBJECT
public:
    explicit StatusDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

protected:
    void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const override;
};

/** Two lines: name + description snippet. */
class NameDelegate : public BaseDelegate
{
    Q_OBJECT
public:
    explicit NameDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

protected:
    void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const override;
};

/** Tag chips. */
class TagsDelegate : public BaseDelegate
{
    Q_OBJECT
public:
    explicit TagsDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

protected:
    void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const override;
};

/** Downloadable check box. */
class CheckDelegate : public BaseDelegate
{
    Q_OBJECT
public:
    explicit CheckDelegate(const ExperimentTableModel *model, QObject *parent = nullptr);

protected:
    void paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const override;
};
