#include "ui/ExperimentDialog.h"

#include "core/Theme.h"
#include "ui/Dialogs.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

namespace {

QWidget *field(const QString &label, QWidget *editor, const QString &placeholder = QString())
{
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    auto *caption = new QLabel(label);
    caption->setObjectName(QStringLiteral("fieldLabel"));

    if (auto *lineEdit = qobject_cast<QLineEdit *>(editor))
        lineEdit->setPlaceholderText(placeholder);

    layout->addWidget(caption);
    layout->addWidget(editor);
    return container;
}

} // namespace

ExperimentDialog::ExperimentDialog(const Experiment &experiment, QWidget *parent)
    : QDialog(parent)
    , m_experiment(experiment)
{
    const bool editing = experiment.id != 0;
    setWindowTitle(editing ? tr("Edit experiment") : tr("New experiment"));
    setModal(true);
    setMinimumSize(560, 620);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(26, 22, 26, 20);
    outer->setSpacing(16);

    // ---- header ------------------------------------------------------------
    auto *title = new QLabel(editing ? tr("Edit experiment") : tr("New experiment"));
    title->setObjectName(QStringLiteral("pageTitle"));

    auto *subtitle = new QLabel(editing
                                    ? tr("Update the details of #%1").arg(experiment.id)
                                    : tr("Register a new project from the lab"));
    subtitle->setObjectName(QStringLiteral("pageSub"));

    outer->addWidget(title);
    outer->addWidget(subtitle);

    // ---- form --------------------------------------------------------------
    auto *form = new QGridLayout;
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(14);
    form->setColumnStretch(0, 1);
    form->setColumnStretch(1, 1);

    m_name = new QLineEdit;
    m_name->setPlaceholderText(tr("e.g. N-Body simulator"));
    form->addWidget(field(tr("Name"), m_name), 0, 0, 1, 2);

    m_status = new QComboBox;
    for (const ExperimentStatus status : Status::all()) {
        m_status->addItem(Status::label(status), int(status));
        m_status->setItemData(m_status->count() - 1, Status::description(status),
                              Qt::ToolTipRole);
    }
    form->addWidget(field(tr("Status"), m_status), 1, 0);

    m_language = new QLineEdit;
    m_language->setPlaceholderText(tr("C++, C#, Python..."));
    form->addWidget(field(tr("Language"), m_language), 1, 1);

    m_framework = new QLineEdit;
    m_framework->setPlaceholderText(tr("Raylib, Qt, Avalonia..."));
    form->addWidget(field(tr("Framework"), m_framework), 2, 0);

    m_engine = new QLineEdit;
    m_engine->setPlaceholderText(tr("Custom, Godot, Unity..."));
    form->addWidget(field(tr("Engine"), m_engine), 2, 1);

    m_description = new QPlainTextEdit;
    m_description->setPlaceholderText(tr("What is this experiment about?"));
    m_description->setFixedHeight(84);
    form->addWidget(field(tr("Description"), m_description), 3, 0, 1, 2);

    auto *pathRow = new QHBoxLayout;
    m_path = new QLineEdit;
    m_path->setPlaceholderText(tr("/path/to/project"));
    auto *browse = new QPushButton(tr("Browse"));
    browse->setObjectName(QStringLiteral("softButton"));
    browse->setCursor(Qt::PointingHandCursor);
    pathRow->addWidget(m_path, 1);
    pathRow->addWidget(browse, 0);

    auto *pathField = new QWidget;
    auto *pathLayout = new QVBoxLayout(pathField);
    pathLayout->setContentsMargins(0, 0, 0, 0);
    pathLayout->setSpacing(5);
    auto *pathCaption = new QLabel(tr("Project path"));
    pathCaption->setObjectName(QStringLiteral("fieldLabel"));
    pathLayout->addWidget(pathCaption);
    pathLayout->addLayout(pathRow);
    form->addWidget(pathField, 4, 0, 1, 2);

    m_tags = new QLineEdit;
    m_tags->setPlaceholderText(tr("space, raylib, gui, simulation"));
    form->addWidget(field(tr("Tags"), m_tags, tr("comma separated")), 5, 0, 1, 2);

    m_notes = new QPlainTextEdit;
    m_notes->setPlaceholderText(tr("Ideas, links, things to remember..."));
    m_notes->setFixedHeight(84);
    form->addWidget(field(tr("Notes"), m_notes), 6, 0, 1, 2);

    auto *options = new QHBoxLayout;
    m_downloadable = new QCheckBox(tr("Downloadable"));
    m_favorite = new QCheckBox(tr("Favourite"));
    options->addWidget(m_downloadable);
    options->addSpacing(18);
    options->addWidget(m_favorite);
    options->addStretch();

    auto *optionsCaption = new QLabel(tr("Options"));
    optionsCaption->setObjectName(QStringLiteral("fieldLabel"));

    auto *optionsBlock = new QVBoxLayout;
    optionsBlock->setSpacing(8);
    optionsBlock->addWidget(optionsCaption);
    optionsBlock->addLayout(options);

    outer->addLayout(form);
    outer->addLayout(optionsBlock);
    outer->addStretch();

    // ---- footer ------------------------------------------------------------
    auto *footer = new QHBoxLayout;
    footer->addStretch();

    auto *cancel = new QPushButton(tr("Cancel"));
    cancel->setObjectName(QStringLiteral("softButton"));
    cancel->setCursor(Qt::PointingHandCursor);
    cancel->setMinimumWidth(104);

    m_saveButton = new QPushButton(editing ? tr("Save changes") : tr("Create experiment"));
    m_saveButton->setObjectName(QStringLiteral("primaryButton"));
    m_saveButton->setCursor(Qt::PointingHandCursor);
    m_saveButton->setMinimumWidth(150);

    footer->addWidget(cancel);
    footer->addWidget(m_saveButton);
    outer->addLayout(footer);

    // ---- wiring ------------------------------------------------------------
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveButton, &QPushButton::clicked, this, &ExperimentDialog::save);
    connect(browse, &QPushButton::clicked, this, &ExperimentDialog::browseFolder);
    connect(m_name, &QLineEdit::returnPressed, this, &ExperimentDialog::save);

    loadValues();
    m_name->setFocus();
}

void ExperimentDialog::loadValues()
{
    m_name->setText(m_experiment.name);
    m_language->setText(m_experiment.language);
    m_framework->setText(m_experiment.framework);
    m_engine->setText(m_experiment.engine);
    m_description->setPlainText(m_experiment.description);
    m_path->setText(m_experiment.projectPath);
    m_tags->setText(m_experiment.tagsDisplay());
    m_notes->setPlainText(m_experiment.notes);
    m_downloadable->setChecked(m_experiment.downloadable);
    m_favorite->setChecked(m_experiment.favorite);

    const int statusIndex = Status::all().indexOf(m_experiment.status);
    m_status->setCurrentIndex(statusIndex >= 0 ? statusIndex : 0);
}

void ExperimentDialog::browseFolder()
{
    const QString start = m_path->text().trimmed().isEmpty()
                              ? QString()
                              : m_path->text().trimmed();

    const QString directory = QFileDialog::getExistingDirectory(
        this, tr("Select project folder"), start,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directory.isEmpty())
        m_path->setText(directory);
}

void ExperimentDialog::save()
{
    const QString name = m_name->text().trimmed();
    if (name.isEmpty()) {
        m_name->setObjectName(QStringLiteral("invalid"));
        m_name->style()->unpolish(m_name);
        m_name->style()->polish(m_name);
        m_name->setFocus();
        Dialogs::message(this, tr("Missing name"),
                         tr("An experiment needs a name before it can be saved."));
        return;
    }

    if (m_name->objectName() != QLatin1String("lineEdit")) {
        m_name->setObjectName(QStringLiteral("lineEdit"));
        m_name->style()->unpolish(m_name);
        m_name->style()->polish(m_name);
    }

    m_experiment.name = name;
    m_experiment.language = m_language->text().trimmed();
    m_experiment.framework = m_framework->text().trimmed();
    m_experiment.engine = m_engine->text().trimmed();
    m_experiment.description = m_description->toPlainText().trimmed();
    m_experiment.projectPath = m_path->text().trimmed();
    m_experiment.notes = m_notes->toPlainText().trimmed();
    m_experiment.downloadable = m_downloadable->isChecked();
    m_experiment.favorite = m_favorite->isChecked();

    m_experiment.tags = splitTags(m_tags->text());

    m_experiment.status = Status::all().value(m_status->currentIndex(),
                                              ExperimentStatus::Planned);

    accept();
}
