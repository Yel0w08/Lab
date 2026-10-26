#include "models/ExperimentFilterProxy.h"
#include "models/ExperimentTableModel.h"

ExperimentFilterProxy::ExperimentFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void ExperimentFilterProxy::setSearchText(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed == m_search)
        return;
    beginFilterChange();
    m_search = trimmed;
    endFilterChange();
}

QString ExperimentFilterProxy::searchText() const
{
    return m_search;
}

void ExperimentFilterProxy::setStatusFilter(std::optional<ExperimentStatus> status)
{
    if (m_status == status)
        return;
    beginFilterChange();
    m_status = status;
    endFilterChange();
}

std::optional<ExperimentStatus> ExperimentFilterProxy::statusFilter() const
{
    return m_status;
}

void ExperimentFilterProxy::setFavoritesOnly(bool on)
{
    if (m_favoritesOnly == on)
        return;
    beginFilterChange();
    m_favoritesOnly = on;
    endFilterChange();
}

bool ExperimentFilterProxy::favoritesOnly() const
{
    return m_favoritesOnly;
}

bool ExperimentFilterProxy::hasActiveFilters() const
{
    return !m_search.isEmpty() || m_status.has_value() || m_favoritesOnly;
}

void ExperimentFilterProxy::clearFilters()
{
    beginFilterChange();
    m_search.clear();
    m_status.reset();
    m_favoritesOnly = false;
    endFilterChange();
}

const Experiment *ExperimentFilterProxy::experimentForRow(int proxyRow) const
{
    const QModelIndex proxyIndex = index(proxyRow, 0);
    if (!proxyIndex.isValid())
        return nullptr;

    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    auto *model = qobject_cast<ExperimentTableModel *>(sourceModel());
    if (!model || !sourceIndex.isValid())
        return nullptr;

    return &model->experiment(sourceIndex.row());
}

QVariant ExperimentFilterProxy::headerData(int section, Qt::Orientation orientation,
                                           int role) const
{
    const QVariant value = QSortFilterProxyModel::headerData(section, orientation, role);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole
        && section == sortColumn() && rowCount() > 0) {
        const QString arrow = sortOrder() == Qt::AscendingOrder
                                  ? QStringLiteral("  ▲")
                                  : QStringLiteral("  ▼");
        return value.toString() + arrow;
    }
    return value;
}

bool ExperimentFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    auto *model = qobject_cast<ExperimentTableModel *>(sourceModel());
    if (!model)
        return true;

    const Experiment &e = model->experiment(sourceRow);

    if (m_favoritesOnly && !e.favorite)
        return false;

    if (m_status.has_value() && e.status != m_status.value())
        return false;

    if (m_search.isEmpty())
        return true;

    const QString needle = m_search;
    if (e.name.contains(needle, Qt::CaseInsensitive)
        || e.description.contains(needle, Qt::CaseInsensitive)
        || e.language.contains(needle, Qt::CaseInsensitive)
        || e.framework.contains(needle, Qt::CaseInsensitive)
        || e.engine.contains(needle, Qt::CaseInsensitive)
        || e.notes.contains(needle, Qt::CaseInsensitive)
        || e.projectPath.contains(needle, Qt::CaseInsensitive))
        return true;

    for (const QString &tag : e.tags) {
        if (tag.contains(needle, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

bool ExperimentFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto *model = qobject_cast<ExperimentTableModel *>(sourceModel());
    if (!model)
        return QSortFilterProxyModel::lessThan(left, right);

    const Experiment &l = model->experiment(left.row());
    const Experiment &r = model->experiment(right.row());

    // Favourites stay pinned on top whatever the sort column is.
    if (l.favorite != r.favorite)
        return l.favorite;

    switch (left.column()) {
    case ExperimentTableModel::ColName:
        return QString::compare(l.name, r.name, Qt::CaseInsensitive) < 0;

    case ExperimentTableModel::ColStatus:
        return QString::compare(Status::label(l.status), Status::label(r.status),
                                Qt::CaseInsensitive) < 0;

    case ExperimentTableModel::ColTags:
        return QString::compare(l.tagsDisplay(), r.tagsDisplay(), Qt::CaseInsensitive) < 0;

    case ExperimentTableModel::ColDownloadable:
        return int(l.downloadable) < int(r.downloadable);

    case ExperimentTableModel::ColModified:
        if (l.lastModified != r.lastModified)
            return l.lastModified < r.lastModified;
        return QString::compare(l.name, r.name, Qt::CaseInsensitive) < 0;

    case ExperimentTableModel::ColFavorite:
    default:
        return QString::compare(l.name, r.name, Qt::CaseInsensitive) < 0;
    }
}
