#pragma once

#include "core/Experiment.h"

#include <QWidget>

class BracketOverlay;
class QEvent;
class QLabel;
class QPushButton;
class TagChipsWidget;

/**
 * Right hand side panel showing everything about the selected experiment.
 * Emits `editRequested` / `favoriteToggled` so the main window can act on it.
 */
class DetailsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DetailsPanel(QWidget *parent = nullptr);

    void showExperiment(const Experiment *experiment);
    void clear();

    bool hasExperiment() const { return m_hasExperiment; }
    int experimentId() const { return m_id; }

signals:
    void editRequested(int id);
    void favoriteToggled(int id);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QWidget *section(const QString &title, QWidget *body);
    void refreshFavoriteStar(const Experiment &experiment);

    BracketOverlay *m_brackets = nullptr;

    QWidget *m_placeholder = nullptr;
    QWidget *m_content = nullptr;

    QLabel *m_statusPill = nullptr;
    QLabel *m_statusHint = nullptr;
    QPushButton *m_star = nullptr;
    QLabel *m_name = nullptr;
    QLabel *m_tech = nullptr;
    QLabel *m_description = nullptr;
    TagChipsWidget *m_tags = nullptr;
    QLabel *m_notes = nullptr;
    QLabel *m_path = nullptr;
    QLabel *m_created = nullptr;
    QLabel *m_modified = nullptr;

    QPushButton *m_openButton = nullptr;
    QPushButton *m_editButton = nullptr;

    QString m_currentPath;
    int m_id = 0;
    bool m_hasExperiment = false;
};
