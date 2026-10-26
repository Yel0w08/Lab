#include "ui/MainWindow.h"

#include "core/Theme.h"
#include "models/ExperimentFilterProxy.h"
#include "models/ExperimentTableModel.h"
#include "ui/Blueprint.h"
#include "ui/Delegates.h"
#include "ui/DetailsPanel.h"
#include "ui/Dialogs.h"
#include "ui/ExperimentDialog.h"
#include "ui/NavItem.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QEvent>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QTableView>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Lab Manager"));
    setMinimumSize(1020, 660);
    resize(1360, 860);

    buildSidebar();
    buildMainArea();

    auto *root = new QWidget;
    auto *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(m_sidebar, 0);
    rootLayout->addWidget(m_main, 1);
    setCentralWidget(root);

    // ---- wiring ------------------------------------------------------------
    connect(m_table->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onSelectionChanged);
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged);

    connect(m_table, &QTableView::clicked, this, &MainWindow::onTableClicked);
    connect(m_table, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(m_table, &QTableView::customContextMenuRequested,
            this, &MainWindow::showTableContextMenu);

    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);

    connect(m_proxy, &QAbstractItemModel::modelReset, this, [this]() {
        updateEmptyState();
        updateActions();
    });
    connect(m_proxy, &QAbstractItemModel::rowsInserted, this,
            [this]() { updateEmptyState(); });
    connect(m_proxy, &QAbstractItemModel::rowsRemoved, this,
            [this]() { updateEmptyState(); });
    connect(m_proxy, &QAbstractItemModel::layoutChanged, this, [this]() {
        m_model->setHoverRow(-1);
        updateEmptyState();
        updateHeader();
    });

    connect(m_table->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
            [this](int column, Qt::SortOrder order) {
                m_proxy->sort(column, order);
            });

    connect(m_details, &DetailsPanel::editRequested, this, &MainWindow::editById);
    connect(m_details, &DetailsPanel::favoriteToggled, this, &MainWindow::toggleFavoriteById);

    applyShortcuts();
    loadSettings();

    QTimer::singleShot(0, this, &MainWindow::reload);
}

// --------------------------------------------------------------------------- setup

void MainWindow::buildSidebar()
{
    const Theme::Palette &palette = Theme::current();

    m_sidebar = new QWidget;
    m_sidebar->setObjectName(QStringLiteral("sidebar"));
    m_sidebar->setFixedWidth(250);

    auto *layout = new QVBoxLayout(m_sidebar);
    layout->setContentsMargins(16, 20, 16, 16);
    layout->setSpacing(4);

    // ---- brand -------------------------------------------------------------
    auto *brand = new QHBoxLayout;
    brand->setSpacing(11);

    auto *mark = new QLabel(QStringLiteral("Δ"));
    mark->setObjectName(QStringLiteral("appMark"));
    mark->setAlignment(Qt::AlignCenter);
    mark->setFixedSize(40, 40);

    auto *brandText = new QVBoxLayout;
    brandText->setSpacing(0);

    auto *appName = new QLabel(tr("Lab Manager"));
    appName->setObjectName(QStringLiteral("appTitle"));

    auto *appSub = new QLabel(tr("Experiments · Qt"));
    appSub->setObjectName(QStringLiteral("appSub"));

    brandText->addWidget(appName);
    brandText->addWidget(appSub);
    brand->addWidget(mark, 0, Qt::AlignTop);
    brand->addLayout(brandText);
    brand->addStretch();

    // ---- library -----------------------------------------------------------
    auto *libraryLabel = new QLabel(tr("LIBRARY"));
    libraryLabel->setObjectName(QStringLiteral("sectionLabel"));

    m_navAll = new NavItem(tr("All experiments"), QColor(palette.accent));
    m_navFavorites = new NavItem(tr("Favourites"), QColor(palette.star));
    m_navAll->setChecked(true);

    connect(m_navAll, &QPushButton::clicked, this, [this]() {
        m_favoritesOnly = false;
        m_statusFilter.reset();
        applySidebarFilter();
    });
    connect(m_navFavorites, &QPushButton::clicked, this, [this]() {
        m_favoritesOnly = true;
        m_statusFilter.reset();
        applySidebarFilter();
    });

    // ---- status ------------------------------------------------------------
    auto *statusLabel = new QLabel(tr("STATUS"));
    statusLabel->setObjectName(QStringLiteral("sectionLabel"));

    const QList<ExperimentStatus> statuses = Status::all();
    for (const ExperimentStatus status : statuses) {
        auto *item = new NavItem(Status::label(status), Status::color(status));
        const ExperimentStatus captured = status;
        connect(item, &QPushButton::clicked, this, [this, captured]() {
            m_favoritesOnly = false;
            m_statusFilter = captured;
            applySidebarFilter();
        });
        m_navStatus.append(item);
    }

    // ---- footer ------------------------------------------------------------
    auto *hints = new QLabel(tr("Ctrl+N new · Ctrl+F search\nF5 refresh · Ctrl+D favourite"));
    hints->setObjectName(QStringLiteral("hint"));
    hints->setWordWrap(true);

    m_themeButton = new QPushButton(Theme::isDark() ? tr("☀  Light theme")
                                                    : tr("☾  Dark theme"));
    m_themeButton->setObjectName(QStringLiteral("softButton"));
    m_themeButton->setCursor(Qt::PointingHandCursor);
    connect(m_themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    layout->addLayout(brand);
    layout->addSpacing(18);
    layout->addWidget(libraryLabel);
    layout->addWidget(m_navAll);
    layout->addWidget(m_navFavorites);
    layout->addSpacing(12);
    layout->addWidget(statusLabel);
    for (NavItem *item : std::as_const(m_navStatus))
        layout->addWidget(item);
    layout->addStretch();
    layout->addWidget(hints);
    layout->addSpacing(8);
    layout->addWidget(m_themeButton);
}

void MainWindow::buildMainArea()
{
    m_main = new BlueprintPanel;
    auto *layout = new QVBoxLayout(m_main);
    layout->setContentsMargins(26, 22, 26, 16);
    layout->setSpacing(14);

    // ---- header ------------------------------------------------------------
    auto *header = new QHBoxLayout;
    header->setSpacing(12);

    auto *titleBox = new QVBoxLayout;
    titleBox->setSpacing(3);
    m_pageTitle = new QLabel;
    m_pageTitle->setObjectName(QStringLiteral("pageTitle"));
    m_pageSub = new QLabel;
    m_pageSub->setObjectName(QStringLiteral("pageSub"));
    titleBox->addWidget(m_pageTitle);
    titleBox->addWidget(m_pageSub);

    m_search = new QLineEdit;
    m_search->setObjectName(QStringLiteral("searchEdit"));
    m_search->setPlaceholderText(tr("Search name, tag, note…"));
    m_search->setClearButtonEnabled(true);
    m_search->setFixedWidth(290);
    m_search->setToolTip(tr("Search everything (Ctrl+F)"));

    m_countBadge = new QLabel;
    m_countBadge->setObjectName(QStringLiteral("countBadge"));
    m_countBadge->setMinimumWidth(90);

    m_addButton = new QPushButton(QStringLiteral("+  ") + tr("New experiment"));
    m_addButton->setObjectName(QStringLiteral("primaryButton"));
    m_addButton->setCursor(Qt::PointingHandCursor);
    m_addButton->setToolTip(tr("Create a new experiment (Ctrl+N)"));
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addExperiment);

    header->addLayout(titleBox);
    header->addStretch();
    header->addWidget(m_search, 0, Qt::AlignVCenter);
    header->addWidget(m_countBadge, 0, Qt::AlignVCenter);
    header->addWidget(m_addButton, 0, Qt::AlignVCenter);

    auto *separator = new QFrame;
    separator->setObjectName(QStringLiteral("separator"));

    // ---- table -------------------------------------------------------------
    auto *content = new QHBoxLayout;
    content->setSpacing(14);

    auto *card = new QFrame;
    card->setObjectName(QStringLiteral("card"));

    auto *grid = new QGridLayout(card);
    grid->setContentsMargins(5, 5, 5, 5);

    m_table = new QTableView;
    setupTable();
    buildEmptyState(card);
    grid->addWidget(m_table, 0, 0);

    m_details = new DetailsPanel;
    m_details->setFixedWidth(338);

    content->addWidget(card, 1);
    content->addWidget(m_details, 0);

    // ---- footer ------------------------------------------------------------
    auto *footer = new QHBoxLayout;
    footer->setSpacing(8);

    m_editButton = new QPushButton(tr("Edit"));
    m_editButton->setCursor(Qt::PointingHandCursor);
    m_editButton->setToolTip(tr("Edit the selected experiment (Ctrl+E)"));
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editSelected);

    m_deleteButton = new QPushButton(tr("Delete"));
    m_deleteButton->setObjectName(QStringLiteral("dangerButton"));
    m_deleteButton->setCursor(Qt::PointingHandCursor);
    m_deleteButton->setToolTip(tr("Delete the selected experiment (Del)"));
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelected);

    m_favoriteButton = new QPushButton(tr("☆  Favourite"));
    m_favoriteButton->setCursor(Qt::PointingHandCursor);
    m_favoriteButton->setToolTip(tr("Toggle favourite (Ctrl+D)"));
    connect(m_favoriteButton, &QPushButton::clicked,
            this, &MainWindow::toggleSelectedFavorite);

    m_downloadableButton = new QPushButton(tr("⇩  Downloadable"));
    m_downloadableButton->setCursor(Qt::PointingHandCursor);
    m_downloadableButton->setToolTip(tr("Toggle the downloadable flag"));
    connect(m_downloadableButton, &QPushButton::clicked,
            this, &MainWindow::toggleSelectedDownloadable);

    m_selectionLabel = new QLabel;
    m_selectionLabel->setObjectName(QStringLiteral("muted"));

    m_statusMessage = new QLabel;
    m_statusMessage->setObjectName(QStringLiteral("hint"));

    auto *refreshButton = new QPushButton(tr("Refresh"));
    refreshButton->setCursor(Qt::PointingHandCursor);
    refreshButton->setToolTip(tr("Reload from the database (F5)"));
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::reload);

    m_detailsButton = new QPushButton(tr("Details"));
    m_detailsButton->setObjectName(QStringLiteral("softButton"));
    m_detailsButton->setCheckable(true);
    m_detailsButton->setCursor(Qt::PointingHandCursor);
    m_detailsButton->setToolTip(tr("Show / hide the details panel (Ctrl+I)"));
    connect(m_detailsButton, &QPushButton::clicked, this, &MainWindow::toggleDetailsPanel);

    footer->addWidget(m_editButton);
    footer->addWidget(m_deleteButton);
    footer->addWidget(m_favoriteButton);
    footer->addWidget(m_downloadableButton);
    footer->addSpacing(10);
    footer->addWidget(m_selectionLabel);
    footer->addStretch();
    footer->addWidget(m_statusMessage);
    footer->addStretch();
    footer->addWidget(m_detailsButton);
    footer->addWidget(refreshButton);

    layout->addLayout(header);
    layout->addWidget(separator);
    layout->addLayout(content, 1);
    layout->addLayout(footer);
}

void MainWindow::setupTable()
{
    m_model = new ExperimentTableModel(this);
    m_proxy = new ExperimentFilterProxy(this);
    m_proxy->setSourceModel(m_model);

    m_table->setModel(m_proxy);
    m_table->setItemDelegate(new BaseDelegate(m_model, m_table));
    m_table->setItemDelegateForColumn(ExperimentTableModel::ColFavorite,
                                      new StarDelegate(m_model, m_table));
    m_table->setItemDelegateForColumn(ExperimentTableModel::ColName,
                                      new NameDelegate(m_model, m_table));
    m_table->setItemDelegateForColumn(ExperimentTableModel::ColStatus,
                                      new StatusDelegate(m_model, m_table));
    m_table->setItemDelegateForColumn(ExperimentTableModel::ColTags,
                                      new TagsDelegate(m_model, m_table));
    m_table->setItemDelegateForColumn(ExperimentTableModel::ColDownloadable,
                                      new CheckDelegate(m_model, m_table));

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(false);
    m_table->setWordWrap(false);
    m_table->setSortingEnabled(false);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    m_table->setFocusPolicy(Qt::StrongFocus);
    m_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(46);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QHeaderView *header = m_table->horizontalHeader();
    header->setHighlightSections(false);
    header->setSectionsClickable(true);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setSectionResizeMode(ExperimentTableModel::ColName, QHeaderView::Stretch);
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_table->setColumnWidth(ExperimentTableModel::ColFavorite, 46);
    m_table->setColumnWidth(ExperimentTableModel::ColStatus, 118);
    m_table->setColumnWidth(ExperimentTableModel::ColTags, 160);
    m_table->setColumnWidth(ExperimentTableModel::ColDownloadable, 44);
    m_table->setColumnWidth(ExperimentTableModel::ColModified, 110);

    m_table->viewport()->setAttribute(Qt::WA_Hover, true);
    m_table->viewport()->setMouseTracking(true);
    m_table->viewport()->installEventFilter(this);

    header->setSortIndicator(ExperimentTableModel::ColModified, Qt::DescendingOrder);
    m_proxy->sort(ExperimentTableModel::ColModified, Qt::DescendingOrder);
}

void MainWindow::buildEmptyState(QFrame *card)
{
    m_emptyState = new QFrame(card);
    m_emptyState->setObjectName(QStringLiteral("emptyState"));

    auto *layout = new QVBoxLayout(m_emptyState);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(8);
    layout->addStretch();

    m_emptyGlyph = new QLabel(QStringLiteral("◎"));
    m_emptyGlyph->setObjectName(QStringLiteral("overlayGlyph"));
    m_emptyGlyph->setAlignment(Qt::AlignCenter);

    m_emptyTitle = new QLabel;
    m_emptyTitle->setObjectName(QStringLiteral("overlayTitle"));
    m_emptyTitle->setAlignment(Qt::AlignCenter);

    m_emptyText = new QLabel;
    m_emptyText->setObjectName(QStringLiteral("overlayText"));
    m_emptyText->setAlignment(Qt::AlignCenter);
    m_emptyText->setWordWrap(true);

    m_emptyAction = new QPushButton;
    m_emptyAction->setCursor(Qt::PointingHandCursor);
    m_emptyAction->setFixedWidth(190);

    connect(m_emptyAction, &QPushButton::clicked, this, [this]() {
        if (m_all.isEmpty()) {
            addExperiment();
            return;
        }
        m_search->clear();
        m_proxy->clearFilters();
        m_favoritesOnly = false;
        m_statusFilter.reset();
        m_navAll->setChecked(true);
        applySidebarFilter();
    });

    layout->addWidget(m_emptyGlyph);
    layout->addWidget(m_emptyTitle);
    layout->addWidget(m_emptyText);
    layout->addSpacing(10);
    layout->addWidget(m_emptyAction, 0, Qt::AlignCenter);
    layout->addStretch();

    m_emptyState->setVisible(false);
}

void MainWindow::applyShortcuts()
{
    auto addWindowAction = [this](const QString &text, const QKeySequence &key,
                                  auto slot, const QString &toolTip = QString()) {
        auto *action = new QAction(text, this);
        action->setShortcut(key);
        action->setToolTip(toolTip);
        action->setShortcutContext(Qt::WindowShortcut);
        addAction(action);
        connect(action, &QAction::triggered, this, slot);
        return action;
    };

    auto addTableAction = [this](const QString &text, const QKeySequence &key,
                                 auto slot, const QString &toolTip = QString()) {
        auto *action = new QAction(text, this);
        action->setShortcut(key);
        action->setToolTip(toolTip);
        action->setAutoRepeat(false);
        action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
        m_table->addAction(action);
        connect(action, &QAction::triggered, this, slot);
        return action;
    };

    addWindowAction(tr("New experiment"), QKeySequence(QStringLiteral("Ctrl+N")),
                    &MainWindow::addExperiment);
    addWindowAction(tr("Search"), QKeySequence(QStringLiteral("Ctrl+F")), [this]() {
        m_search->setFocus();
        m_search->selectAll();
    });
    addWindowAction(tr("Refresh"), QKeySequence(QStringLiteral("F5")), &MainWindow::reload);
    addWindowAction(tr("Details panel"), QKeySequence(QStringLiteral("Ctrl+I")),
                    &MainWindow::toggleDetailsPanel);

    addTableAction(tr("Edit"), QKeySequence(QStringLiteral("Ctrl+E")),
                   &MainWindow::editSelected);
    addTableAction(tr("Edit"), QKeySequence(Qt::Key_Return), &MainWindow::editSelected);
    addTableAction(tr("Delete"), QKeySequence(Qt::Key_Delete), &MainWindow::deleteSelected);
    addTableAction(tr("Favourite"), QKeySequence(QStringLiteral("Ctrl+D")),
                   &MainWindow::toggleSelectedFavorite);
    addTableAction(tr("Downloadable"), QKeySequence(QStringLiteral("Ctrl+U")),
                   &MainWindow::toggleSelectedDownloadable);
    addTableAction(tr("Open project"), QKeySequence(QStringLiteral("Ctrl+O")),
                   &MainWindow::openSelectedProject);
}

// ------------------------------------------------------------------ data flows

void MainWindow::reload()
{
    const int previousId = m_selectedId;

    m_all = m_service.getAll();
    m_model->setExperiments(m_all);

    updateCounts();
    updateHeader();
    updateEmptyState();
    restoreSelection(previousId);
    updateActions();
}

void MainWindow::restoreSelection(int id)
{
    auto *selection = m_table->selectionModel();
    if (!selection)
        return;

    int row = -1;
    if (id > 0) {
        for (int i = 0; i < m_proxy->rowCount(); ++i) {
            if (m_proxy->data(m_proxy->index(i, 0), ExperimentTableModel::IdRole).toInt()
                == id) {
                row = i;
                break;
            }
        }
    }
    if (row < 0 && m_proxy->rowCount() > 0)
        row = 0;

    if (row < 0) {
        selection->clear();
        m_selectedId = 0;
        m_details->clear();
        return;
    }

    const QModelIndex index = m_proxy->index(row, 0);
    selection->setCurrentIndex(index,
                               QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    m_table->scrollTo(index, QAbstractItemView::EnsureVisible);
    updateDetails();
}

void MainWindow::applySidebarFilter()
{
    m_proxy->setStatusFilter(m_statusFilter);
    m_proxy->setFavoritesOnly(m_favoritesOnly);

    updateHeader();
    updateEmptyState();
    updateActions();
}

void MainWindow::onSearchChanged(const QString &text)
{
    m_proxy->setSearchText(text);
    updateHeader();
    updateEmptyState();
    updateActions();
}

void MainWindow::onSelectionChanged()
{
    updateDetails();
    updateActions();
}

void MainWindow::updateDetails()
{
    const QModelIndex current = m_table->currentIndex();
    if (!current.isValid()) {
        m_selectedId = 0;
        m_details->clear();
        return;
    }

    const Experiment *experiment = m_proxy->experimentForRow(current.row());
    if (!experiment) {
        m_selectedId = 0;
        m_details->clear();
        return;
    }

    m_selectedId = experiment->id;
    m_details->showExperiment(experiment);
}

void MainWindow::refreshDetailsPanel()
{
    if (m_selectedId <= 0)
        return;
    if (const auto experiment = m_service.getById(m_selectedId))
        m_details->showExperiment(&experiment.value());
}

void MainWindow::updateCounts()
{
    int favorites = 0;
    QList<int> perStatus(Status::all().size(), 0);

    for (const Experiment &experiment : std::as_const(m_all)) {
        if (experiment.favorite)
            ++favorites;
        const int index = Status::all().indexOf(experiment.status);
        if (index >= 0)
            ++perStatus[index];
    }

    m_navAll->setCount(m_all.size());
    m_navFavorites->setCount(favorites);
    for (int i = 0; i < m_navStatus.size() && i < perStatus.size(); ++i)
        m_navStatus[i]->setCount(perStatus[i]);
}

void MainWindow::updateHeader()
{
    m_pageTitle->setText(sectionTitle());

    QString subtitle;
    if (m_statusFilter.has_value())
        subtitle = Status::description(m_statusFilter.value());
    else if (m_favoritesOnly)
        subtitle = tr("Pinned experiments, always on top");
    else
        subtitle = tr("Everything in the lab");

    m_pageSub->setText(subtitle);

    const int total = m_all.size();
    const int shown = m_proxy->rowCount();
    m_countBadge->setText(shown == total
                              ? tr("%1 experiment(s)").arg(total)
                              : tr("%1 of %2 shown").arg(shown).arg(total));
    m_countBadge->setToolTip(tr("Showing %1 of %2 experiments").arg(shown).arg(total));
}

void MainWindow::updateEmptyState()
{
    const bool empty = m_proxy->rowCount() == 0;
    m_emptyState->setVisible(empty);
    if (!empty)
        return;

    if (m_all.isEmpty()) {
        m_emptyGlyph->setText(QStringLiteral("＋"));
        m_emptyTitle->setText(tr("No experiments yet"));
        m_emptyText->setText(tr("Create your very first experiment to start filling the lab."));
        m_emptyAction->setText(tr("New experiment"));
    } else {
        m_emptyGlyph->setText(QStringLiteral("◎"));
        m_emptyTitle->setText(tr("Nothing matches"));
        m_emptyText->setText(tr("Try another search or clear the active filters."));
        m_emptyAction->setText(tr("Clear filters"));
    }

    const QWidget *parent = m_emptyState->parentWidget();
    if (!parent || !m_table)
        return;

    const QPoint origin = m_table->mapTo(parent, QPoint(0, 0));
    const int headerHeight = m_table->horizontalHeader()
                                 ? m_table->horizontalHeader()->height()
                                 : 0;
    m_emptyState->setGeometry(QRect(origin.x(), origin.y() + headerHeight,
                                    m_table->width(),
                                    qMax(0, m_table->height() - headerHeight)));
}

void MainWindow::updateActions()
{
    const bool hasSelection = m_selectedId > 0;

    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
    m_favoriteButton->setEnabled(hasSelection);
    m_downloadableButton->setEnabled(hasSelection);

    const auto experiment = selectedExperiment();
    if (experiment.has_value()) {
        m_favoriteButton->setText(experiment->favorite
                                      ? QStringLiteral("★  Favourite")
                                      : QStringLiteral("☆  Favourite"));
        m_downloadableButton->setStyleSheet(
            experiment->downloadable
                ? QStringLiteral("QPushButton { color: %1; font-weight: 700; }")
                      .arg(Theme::current().accent)
                : QString());
        m_selectionLabel->setText(tr("Selected · %1").arg(experiment->name));
    } else {
        m_favoriteButton->setText(QStringLiteral("☆  Favourite"));
        m_downloadableButton->setStyleSheet({});
        m_selectionLabel->setText(tr("Nothing selected"));
    }
}

QString MainWindow::sectionTitle() const
{
    const QString label = [this]() {
        if (m_favoritesOnly)
            return tr("Favourites");
        if (m_statusFilter.has_value())
            return Status::label(m_statusFilter.value());
        return tr("All experiments");
    }();
    return label;
}

int MainWindow::selectedId() const
{
    return m_selectedId;
}

std::optional<Experiment> MainWindow::selectedExperiment() const
{
    if (m_selectedId <= 0)
        return std::nullopt;
    return m_service.getById(m_selectedId);
}

std::optional<Experiment> MainWindow::experimentById(int id) const
{
    if (id <= 0)
        return std::nullopt;
    return m_service.getById(id);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_statusMessage->setText(message);
    QTimer::singleShot(3500, this, [this, message]() {
        if (m_statusMessage->text() == message)
            m_statusMessage->clear();
    });
}

// ----------------------------------------------------------------- interactions

void MainWindow::onTableClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const int id = m_proxy->data(index, ExperimentTableModel::IdRole).toInt();
    if (id <= 0)
        return;

    if (index.column() == ExperimentTableModel::ColFavorite) {
        toggleFavoriteById(id);
    } else if (index.column() == ExperimentTableModel::ColDownloadable) {
        toggleDownloadableById(id);
    }
}

void MainWindow::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (index.column() == ExperimentTableModel::ColFavorite
        || index.column() == ExperimentTableModel::ColDownloadable)
        return;

    editSelected();
}

void MainWindow::addExperiment()
{
    ExperimentDialog dialog(Experiment(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    Experiment created = dialog.experiment();
    if (!m_service.add(created)) {
        Dialogs::message(this, tr("Save failed"), m_service.lastError());
        return;
    }

    m_selectedId = created.id;
    reload();
    showStatusMessage(tr("“%1” created").arg(created.name));
}

void MainWindow::editSelected()
{
    editById(selectedId());
}

void MainWindow::editById(int id)
{
    if (id <= 0) {
        Dialogs::message(this, tr("Nothing selected"),
                         tr("Pick an experiment in the list first."));
        return;
    }

    const auto existing = experimentById(id);
    if (!existing.has_value()) {
        Dialogs::message(this, tr("Not found"), tr("This experiment no longer exists."));
        return;
    }

    ExperimentDialog dialog(existing.value(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    Experiment updated = dialog.experiment();
    if (!m_service.update(updated)) {
        Dialogs::message(this, tr("Save failed"), m_service.lastError());
        return;
    }

    m_selectedId = id;
    reload();
    showStatusMessage(tr("“%1” saved").arg(updated.name));
}

void MainWindow::deleteSelected()
{
    const int id = selectedId();
    if (id <= 0) {
        Dialogs::message(this, tr("Nothing selected"),
                         tr("Pick an experiment to delete."));
        return;
    }

    const auto experiment = experimentById(id);
    const QString name = experiment.has_value() ? experiment->name : QString::number(id);

    if (!Dialogs::confirm(this, tr("Delete experiment"),
                          tr("Delete “%1”? This cannot be undone.").arg(name),
                          tr("Delete"), true))
        return;

    if (!m_service.remove(id)) {
        Dialogs::message(this, tr("Delete failed"), m_service.lastError());
        return;
    }

    if (m_selectedId == id)
        m_selectedId = 0;

    reload();
    showStatusMessage(tr("“%1” deleted").arg(name));
}

void MainWindow::toggleSelectedFavorite()
{
    toggleFavoriteById(selectedId());
}

void MainWindow::toggleFavoriteById(int id)
{
    const auto experiment = experimentById(id);
    if (!experiment.has_value())
        return;

    Experiment updated = experiment.value();
    updated.favorite = !updated.favorite;
    if (!m_service.update(updated)) {
        Dialogs::message(this, tr("Update failed"), m_service.lastError());
        return;
    }

    reload();
    showStatusMessage(updated.favorite ? tr("“%1” added to favourites").arg(updated.name)
                                       : tr("“%1” removed from favourites").arg(updated.name));
}

void MainWindow::toggleSelectedDownloadable()
{
    toggleDownloadableById(selectedId());
}

void MainWindow::toggleDownloadableById(int id)
{
    const auto experiment = experimentById(id);
    if (!experiment.has_value())
        return;

    Experiment updated = experiment.value();
    updated.downloadable = !updated.downloadable;
    if (!m_service.update(updated)) {
        Dialogs::message(this, tr("Update failed"), m_service.lastError());
        return;
    }

    reload();
    showStatusMessage(updated.downloadable
                          ? tr("“%1” marked downloadable").arg(updated.name)
                          : tr("“%1” is no longer downloadable").arg(updated.name));
}

void MainWindow::openSelectedProject()
{
    const auto experiment = selectedExperiment();
    if (!experiment.has_value()) {
        Dialogs::message(this, tr("Nothing selected"),
                         tr("Pick an experiment first."));
        return;
    }

    const QString path = experiment->projectPath.trimmed();
    if (path.isEmpty()) {
        Dialogs::message(this, tr("No project path"),
                         tr("This experiment has no project folder set."));
        return;
    }

    if (!QFileInfo::exists(path)) {
        Dialogs::message(this, tr("Folder missing"),
                         tr("The folder does not exist:\n%1").arg(path));
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::showTableContextMenu(const QPoint &position)
{
    const QModelIndex index = m_table->indexAt(position);
    if (index.isValid())
        m_table->selectRow(index.row());

    if (m_selectedId <= 0)
        return;

    const auto experiment = selectedExperiment();
    const bool hasPath = experiment.has_value() && experiment->hasProjectPath();

    QMenu menu(this);
    menu.addAction(tr("Edit…"), this, &MainWindow::editSelected);
    menu.addAction(tr("Toggle favourite"), this, &MainWindow::toggleSelectedFavorite);
    menu.addAction(tr("Toggle downloadable"), this, &MainWindow::toggleSelectedDownloadable);

    QAction *open = menu.addAction(tr("Open project folder"), this,
                                   &MainWindow::openSelectedProject);
    open->setEnabled(hasPath);

    menu.addSeparator();
    menu.addAction(tr("Delete…"), this, &MainWindow::deleteSelected);

    menu.exec(m_table->viewport()->mapToGlobal(position));
}

// ------------------------------------------------------------------- appearance

void MainWindow::toggleTheme()
{
    Theme::toggle(*qApp);
    QSettings settings(QStringLiteral("Yel0w"), QStringLiteral("LabManager"));
    settings.setValue(QStringLiteral("theme"),
                      Theme::isDark() ? QStringLiteral("dark") : QStringLiteral("light"));

    m_themeButton->setText(Theme::isDark() ? tr("☀  Light theme") : tr("☾  Dark theme"));

    const auto widgets = findChildren<QWidget *>();
    for (QWidget *widget : widgets)
        widget->update();

    refreshDetailsPanel();
    updateActions();
}

void MainWindow::toggleDetailsPanel()
{
    const bool visible = !m_details->isVisible();
    m_details->setVisible(visible);
    m_detailsButton->setChecked(visible);

    QSettings settings(QStringLiteral("Yel0w"), QStringLiteral("LabManager"));
    settings.setValue(QStringLiteral("detailsVisible"), visible);
}

// --------------------------------------------------------------------- events

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_table->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            const int row = m_table->rowAt(int(mouseEvent->position().y()));
            if (row != m_model->hoverRow()) {
                m_model->setHoverRow(row);
                m_table->viewport()->update();
            }
        } else if (event->type() == QEvent::Leave) {
            if (m_model->hoverRow() != -1) {
                m_model->setHoverRow(-1);
                m_table->viewport()->update();
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateEmptyState();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings(QStringLiteral("Yel0w"), QStringLiteral("LabManager"));

    const bool detailsVisible = settings.value(QStringLiteral("detailsVisible"), true).toBool();
    m_details->setVisible(detailsVisible);
    m_detailsButton->setChecked(detailsVisible);

    if (settings.contains(QStringLiteral("geometry")))
        restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings(QStringLiteral("Yel0w"), QStringLiteral("LabManager"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("detailsVisible"), m_details->isVisible());
}
