#pragma once

#include "core/Experiment.h"

#include <QAbstractTableModel>

/**
 * Read only table model backing the experiment list.
 * Custom roles carry the raw values so the delegates can paint status pills,
 * tag chips, stars and check boxes.
 */
class ExperimentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColFavorite = 0,
        ColName,
        ColStatus,
        ColTags,
        ColDownloadable,
        ColModified,
        ColumnCount
    };

    enum Role {
        IdRole = Qt::UserRole + 1,
        FavoriteRole,
        StatusRole,
        TagsRole,
        DownloadableRole,
        ModifiedRole
    };

    explicit ExperimentTableModel(QObject *parent = nullptr);

    void setExperiments(QList<Experiment> experiments);
    const Experiment &experiment(int row) const;
    int rowForId(int id) const;

    /** Row hovered with the mouse (proxy row), -1 when nothing is hovered. */
    int hoverRow() const;
    void setHoverRow(int row);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<Experiment> m_items;
    int m_hoverRow = -1;
};
