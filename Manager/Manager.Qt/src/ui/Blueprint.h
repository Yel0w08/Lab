#pragma once

#include <QFrame>
#include <QWidget>

/**
 * Blueprint grid backdrop for the main workspace: faint engineering grid lines
 * with bolder axis lines every five cells, plus cyan registration marks at the
 * corners — so the whole panel reads like a laboratory / schematic sheet.
 */
class BlueprintPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BlueprintPanel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/**
 * A card frame with squared (machined) corners and four targeting brackets
 * painted along its corners. The engineered bevel itself is applied by the
 * `QFrame#card` stylesheet rule.
 */
class LabFrame : public QFrame
{
    Q_OBJECT

public:
    explicit LabFrame(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/**
 * A transparent widget painted with four targeting brackets at and around its
 * corners. Raise it above a card's children (then resize it together with the
 * card) so the brackets stay visible on top of opaque content.
 */
class BracketOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit BracketOverlay(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};