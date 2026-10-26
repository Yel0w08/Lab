#pragma once

#include "core/Experiment.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

/**
 * Create / edit dialog for an experiment.
 * `id == 0` means "new experiment".
 */
class ExperimentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExperimentDialog(const Experiment &experiment, QWidget *parent = nullptr);

    Experiment experiment() const { return m_experiment; }

private slots:
    void browseFolder();
    void save();

private:
    void loadValues();
    QWidget *labeledEdit(const QString &label, QLineEdit **edit);
    QWidget *labeledArea(const QString &label, QPlainTextEdit **edit);

    Experiment m_experiment;

    QLineEdit *m_name = nullptr;
    QComboBox *m_status = nullptr;
    QLineEdit *m_language = nullptr;
    QLineEdit *m_framework = nullptr;
    QLineEdit *m_engine = nullptr;
    QPlainTextEdit *m_description = nullptr;
    QLineEdit *m_path = nullptr;
    QLineEdit *m_tags = nullptr;
    QPlainTextEdit *m_notes = nullptr;
    QCheckBox *m_downloadable = nullptr;
    QCheckBox *m_favorite = nullptr;
    QPushButton *m_saveButton = nullptr;
};
