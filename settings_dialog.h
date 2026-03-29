#pragma once
#include <QDialog>
#include "app_settings.h"

class QComboBox;
class QFontComboBox;
class QSpinBox;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(const AppSettings &current, QWidget *parent = nullptr);
    AppSettings settings() const;

private:
    QComboBox     *m_theme;
    QFontComboBox *m_displayFont;
    QSpinBox      *m_displayFontSize;
    QFontComboBox *m_buttonFont;
    QSpinBox      *m_buttonFontSize;
};
