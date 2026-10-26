#pragma once

#include "core/Experiment.h"

#include <QSortFilterProxyModel>
#include <optional>

/**
 * Live filtering (search text / status / favourites) + sorting.
 * Favourites are always pinned on top, then the active sort column applies.
 */
class ExperimentFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ExperimentFilterProxy(QObject *parent = nullptr);

    void setSearchText(const QString &text);
    QString searchText() const;

    void setStatusFilter(std::optional<ExperimentStatus> status);
    std::optional<ExperimentStatus> statusFilter() const;

    void setFavoritesOnly(bool on);
    bool favoritesOnly() const;

    bool hasActiveFilters() const;
    void clearFilters();

    /** Experiment shown on the given (proxy) row, or nullptr. */
    const Experiment *experimentForRow(int proxyRow) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_search;
    std::optional<ExperimentStatus> m_status;
    bool m_favoritesOnly = false;
};
