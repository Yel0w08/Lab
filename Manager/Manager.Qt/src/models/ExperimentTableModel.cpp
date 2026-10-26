#include "models/ExperimentTableModel.h"

#include <QLocale>

namespace {
const Experiment &emptyExperiment()
{
    static const Experiment empty;
    return empty;
}
} // namespace

ExperimentTableModel::ExperimentTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void ExperimentTableModel::setExperiments(QList<Experiment> experiments)
{
    beginResetModel();
    m_items = std::move(experiments);
    m_hoverRow = -1;
    endResetModel();
}

const Experiment &ExperimentTableModel::experiment(int row) const
{
    if (row < 0 || row >= m_items.size())
        return emptyExperiment();
    return m_items.at(row);
}

int ExperimentTableModel::rowForId(int id) const
{
    for (int row = 0; row < m_items.size(); ++row) {
        if (m_items.at(row).id == id)
            return row;
    }
    return -1;
}

int ExperimentTableModel::hoverRow() const
{
    return m_hoverRow;
}

void ExperimentTableModel::setHoverRow(int row)
{
    if (m_hoverRow == row)
        return;
    m_hoverRow = row;
}

int ExperimentTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

int ExperimentTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant ExperimentTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const Experiment &e = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case ColFavorite:      return e.favorite ? QStringLiteral("★") : QStringLiteral("☆");
        case ColName:          return e.name;
        case ColStatus:        return Status::label(e.status);
        case ColTags:          return e.tagsDisplay();
        case ColDownloadable:  return e.downloadable ? QStringLiteral("Yes") : QStringLiteral("No");
        case ColModified:      return QLocale().toString(e.lastModified, QLocale::ShortFormat);
        default: break;
        }
        break;

    case Qt::ToolTipRole:
        switch (index.column()) {
        case ColName: {
            QString tip = e.name;
            if (!e.description.isEmpty())
                tip += QStringLiteral("\n") + e.description;
            return tip;
        }
        case ColStatus:
            return Status::label(e.status) + QStringLiteral(" — ")
                   + Status::description(e.status);
        case ColTags:
            return e.tagsDisplay();
        case ColModified:
            return e.lastModified.toString(QStringLiteral("dddd, d MMMM yyyy, hh:mm"));
        default:
            break;
        }
        break;

    case Qt::TextAlignmentRole:
        if (index.column() == ColModified)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        return int(Qt::AlignLeft | Qt::AlignVCenter);

    case IdRole:          return e.id;
    case FavoriteRole:    return e.favorite;
    case StatusRole:      return int(e.status);
    case TagsRole:        return e.tags;
    case DownloadableRole: return e.downloadable;
    case ModifiedRole:    return e.lastModified;
    default: break;
    }

    return {};
}

QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation,
                                          int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case ColFavorite:      return QStringLiteral("☆");
    case ColName:          return QStringLiteral("NAME");
    case ColStatus:        return QStringLiteral("STATUS");
    case ColTags:          return QStringLiteral("TAGS");
    case ColDownloadable:  return QStringLiteral("⇩");
    case ColModified:      return QStringLiteral("MODIFIED");
    default: break;
    }
    return {};
}

Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
