#include "settings_dialog.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QFontComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(const AppSettings &s, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Preferences");
    setMinimumWidth(380);

    auto *form = new QFormLayout();
    form->setSpacing(10);
    form->setContentsMargins(16, 16, 16, 8);
    form->setLabelAlignment(Qt::AlignRight);

    m_theme = new QComboBox(this);
    m_theme->addItem("Dark",  "dark");
    m_theme->addItem("Light", "light");
    m_theme->setCurrentIndex(s.theme == "light" ? 1 : 0);
    form->addRow("Theme:", m_theme);

    m_displayFont = new QFontComboBox(this);
    m_displayFont->setCurrentFont(QFont(s.displayFont));
    m_displayFontSize = new QSpinBox(this);
    m_displayFontSize->setRange(12, 72);
    m_displayFontSize->setValue(s.displayFontSize);
    m_displayFontSize->setSuffix(" pt");
    auto *dispRow = new QHBoxLayout();
    dispRow->addWidget(m_displayFont, 1);
    dispRow->addWidget(m_displayFontSize);
    form->addRow("Display font:", dispRow);

    m_buttonFont = new QFontComboBox(this);
    m_buttonFont->setCurrentFont(QFont(s.buttonFont));
    m_buttonFontSize = new QSpinBox(this);
    m_buttonFontSize->setRange(8, 36);
    m_buttonFontSize->setValue(s.buttonFontSize);
    m_buttonFontSize->setSuffix(" pt");
    auto *btnRow = new QHBoxLayout();
    btnRow->addWidget(m_buttonFont, 1);
    btnRow->addWidget(m_buttonFontSize);
    form->addRow("Button font:", btnRow);

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addSpacing(8);
    root->addWidget(buttonBox);
    root->setContentsMargins(0, 0, 0, 12);
}

AppSettings SettingsDialog::settings() const {
    AppSettings s;
    s.theme           = m_theme->currentData().toString();
    s.displayFont     = m_displayFont->currentFont().family();
    s.displayFontSize = m_displayFontSize->value();
    s.buttonFont      = m_buttonFont->currentFont().family();
    s.buttonFontSize  = m_buttonFontSize->value();
    return s;
}
