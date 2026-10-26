#pragma once

#include "core/Experiment.h"
#include "core/ExperimentService.h"

#include <QMainWindow>
#include <QList>
#include <optional>

class DetailsPanel;
class ExperimentFilterProxy;
class ExperimentTableModel;
class NavItem;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class QFrame;

/**
 * Main window: navigation rail (library + status filters), search header,
 * experiment table with a details panel, and an action footer.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void reload();
    void applySidebarFilter();
    void onSearchChanged(const QString &text);
    void onSelectionChanged();
    void onTableClicked(const QModelIndex &index);
    void onTableDoubleClicked(const QModelIndex &index);

    void addExperiment();
    void editSelected();
    void deleteSelected();
    void toggleSelectedFavorite();
    void toggleSelectedDownloadable();
    void openSelectedProject();
    void showTableContextMenu(const QPoint &position);

    void toggleTheme();
    void toggleDetailsPanel();

    void editById(int id);
    void toggleFavoriteById(int id);
    void toggleDownloadableById(int id);

private:
    void buildSidebar();
    void buildMainArea();
    void buildEmptyState(QFrame *card);
    void setupTable();
    void applyShortcuts();

    void updateCounts();
    void updateHeader();
    void updateEmptyState();
    void updateActions();
    void updateDetails();
    void updateOverlayAction();

    int selectedId() const;
    std::optional<Experiment> selectedExperiment() const;
    std::optional<Experiment> experimentById(int id) const;
    void restoreSelection(int id);
    void showStatusMessage(const QString &message);
    void refreshDetailsPanel();
    void loadSettings();
    void saveSettings();

    QString sectionTitle() const;

    // data
    ExperimentService m_service;
    QList<Experiment> m_all;
    bool m_favoritesOnly = false;
    std::optional<ExperimentStatus> m_statusFilter;
    int m_selectedId = 0;

    // widgets
    QWidget *m_main = nullptr;
    QWidget *m_sidebar = nullptr;
    NavItem *m_navAll = nullptr;
    NavItem *m_navFavorites = nullptr;
    QList<NavItem *> m_navStatus;

    QLabel *m_pageTitle = nullptr;
    QLabel *m_pageSub = nullptr;
    QLabel *m_countBadge = nullptr;
    QLineEdit *m_search = nullptr;

    ExperimentTableModel *m_model = nullptr;
    ExperimentFilterProxy *m_proxy = nullptr;
    QTableView *m_table = nullptr;
    QFrame *m_emptyState = nullptr;
    QLabel *m_emptyGlyph = nullptr;
    QLabel *m_emptyTitle = nullptr;
    QLabel *m_emptyText = nullptr;
    QPushButton *m_emptyAction = nullptr;

    DetailsPanel *m_details = nullptr;

    QLabel *m_statusMessage = nullptr;
    QLabel *m_selectionLabel = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_deleteButton = nullptr;
    QPushButton *m_favoriteButton = nullptr;
    QPushButton *m_downloadableButton = nullptr;
    QPushButton *m_detailsButton = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_themeButton = nullptr;
};
