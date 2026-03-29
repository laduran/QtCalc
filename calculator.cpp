#include "calculator.h"
#include "settings_dialog.h"
#include <QGridLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>

Calculator::Calculator(const QString &themeOverride, QWidget *parent) : QMainWindow(parent) {
    loadSettings();
    if (!themeOverride.isEmpty())
        m_settings.theme = themeOverride;

    auto *settingsMenu = menuBar()->addMenu("&Settings");
    settingsMenu->addAction("&Preferences\u2026", this, &Calculator::openSettings);

    auto *central = new QWidget(this);
    setCentralWidget(central);
    central->setFixedWidth(280);

    auto *root = new QVBoxLayout(central);
    root->setSpacing(4);
    root->setContentsMargins(8, 8, 8, 8);

    m_display = new QLineEdit("0", central);
    m_display->setReadOnly(true);
    m_display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_display->setFixedHeight(72);
    root->addWidget(m_display);

    auto *grid = new QGridLayout();
    grid->setSpacing(4);

    struct Btn { QString label; int row, col, rowSpan, colSpan; };
    const QList<Btn> buttons = {
        {"C",  0, 0, 1, 1}, {"\u00f7", 0, 1, 1, 1}, {"\u00d7", 0, 2, 1, 1}, {"AC", 0, 3, 1, 1},
        {"7",  1, 0, 1, 1}, {"8",      1, 1, 1, 1}, {"9",      1, 2, 1, 1}, {"-",  1, 3, 1, 1},
        {"4",  2, 0, 1, 1}, {"5",      2, 1, 1, 1}, {"6",      2, 2, 1, 1}, {"+",  2, 3, 1, 1},
        {"1",  3, 0, 1, 1}, {"2",      3, 1, 1, 1}, {"3",      3, 2, 1, 1}, {"=",  3, 3, 2, 1},
        {"0",  4, 0, 1, 2}, {".",      4, 2, 1, 1},
    };

    for (const auto &b : buttons) {
        auto *btn = new QPushButton(b.label, central);
        grid->addWidget(btn, b.row, b.col, b.rowSpan, b.colSpan);

        const QString label = b.label;
        if (label >= "0" && label <= "9") {
            connect(btn, &QPushButton::clicked, this, [this, label] { digitPressed(label); });
        } else if (label == ".") {
            connect(btn, &QPushButton::clicked, this, [this] { decimalPressed(); });
        } else if (label == "=") {
            connect(btn, &QPushButton::clicked, this, [this] { equalsPressed(); });
        } else if (label == "C") {
            connect(btn, &QPushButton::clicked, this, [this] { clearEntry(); });
        } else if (label == "AC") {
            connect(btn, &QPushButton::clicked, this, [this] { allClear(); });
        } else {
            connect(btn, &QPushButton::clicked, this, [this, label] { operatorPressed(label); });
        }
    }

    root->addLayout(grid);
    applySettings();
}

// ── Calculator logic ────────────────────────────────────────────────────────

void Calculator::digitPressed(const QString &digit) {
    if (m_waitingForOperand) {
        m_display->setText(digit);
        m_waitingForOperand = false;
    } else {
        const QString cur = m_display->text();
        m_display->setText(cur == "0" ? digit : cur + digit);
    }
}

void Calculator::operatorPressed(const QString &op) {
    if (m_display->text() == "Error") return;

    const double value = m_display->text().toDouble();

    if (!m_pendingOp.isEmpty() && !m_waitingForOperand) {
        double result = 0.0;
        if      (m_pendingOp == "+")          result = m_pendingValue + value;
        else if (m_pendingOp == "-")          result = m_pendingValue - value;
        else if (m_pendingOp == "\u00d7")     result = m_pendingValue * value;
        else if (m_pendingOp == "\u00f7") {
            if (value == 0.0) {
                m_display->setText("Error");
                m_pendingOp.clear();
                m_waitingForOperand = true;
                return;
            }
            result = m_pendingValue / value;
        }
        m_display->setText(formatResult(result));
        m_pendingValue = result;
    } else {
        m_pendingValue = value;
    }

    m_pendingOp        = op;
    m_waitingForOperand = true;
}

void Calculator::equalsPressed() {
    if (m_pendingOp.isEmpty() || m_display->text() == "Error") return;

    const double value = m_display->text().toDouble();
    double result = 0.0;

    if      (m_pendingOp == "+")          result = m_pendingValue + value;
    else if (m_pendingOp == "-")          result = m_pendingValue - value;
    else if (m_pendingOp == "\u00d7")     result = m_pendingValue * value;
    else if (m_pendingOp == "\u00f7") {
        if (value == 0.0) {
            m_display->setText("Error");
            m_pendingOp.clear();
            m_waitingForOperand = true;
            return;
        }
        result = m_pendingValue / value;
    }

    m_display->setText(formatResult(result));
    m_pendingOp.clear();
    m_waitingForOperand = true;
}

void Calculator::decimalPressed() {
    if (m_waitingForOperand) {
        m_display->setText("0.");
        m_waitingForOperand = false;
        return;
    }
    if (!m_display->text().contains('.'))
        m_display->setText(m_display->text() + ".");
}

void Calculator::clearEntry() {
    m_display->setText("0");
    m_waitingForOperand = true;
}

void Calculator::allClear() {
    m_display->setText("0");
    m_pendingValue     = 0.0;
    m_pendingOp.clear();
    m_waitingForOperand = true;
}

QString Calculator::formatResult(double value) const {
    if (std::isinf(value) || std::isnan(value)) return "Error";
    if (value == std::floor(value) && std::abs(value) < 1e15)
        return QString::number(static_cast<long long>(value));
    return QString::number(value, 'g', 10);
}

// ── Settings ────────────────────────────────────────────────────────────────

void Calculator::openSettings() {
    SettingsDialog dlg(m_settings, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_settings = dlg.settings();
        saveSettings();
        applySettings();
    }
}

void Calculator::applySettings() {
    setStyleSheet(buildStyleSheet());
}

QString Calculator::buildStyleSheet() const {
    const bool d = (m_settings.theme == "dark");

    const QString bg      = d ? "#1a1d2b" : "#f0f2f5";
    const QString dispBg  = d ? "#0d1017" : "#ffffff";
    const QString dispFg  = d ? "lightgray" : "#111111";
    const QString btnBg   = d ? "#252838" : "#e0e3ef";
    const QString fg      = d ? "white"   : "#111111";
    const QString border  = d ? "#313449" : "#c0c3d0";
    const QString hover   = d ? "#2d3048" : "#cdd0df";
    const QString pressed = d ? "#1c1e30" : "#b8bccf";

    QString ss;
    ss += QString("QMainWindow, QWidget    { background-color: %1; color: %2; }\n").arg(bg, fg);
    ss += QString("QMenuBar               { background-color: %1; color: %2; }\n").arg(bg, fg);
    ss += QString("QMenuBar::item:selected { background-color: %1; }\n").arg(hover);
    ss += QString("QMenu { background-color: %1; color: %2; border: 1px solid %3; }\n").arg(bg, fg, border);
    ss += QString("QMenu::item:selected   { background-color: %1; }\n").arg(hover);
    ss += QString("QLabel                 { background-color: transparent; color: %1; }\n").arg(fg);
    ss += QString("QLineEdit { background-color: %1; color: %2; border: none;"
                  " font-family: \"%3\"; font-size: %4px; padding: 8px 12px; }\n")
            .arg(dispBg, dispFg, m_settings.displayFont)
            .arg(m_settings.displayFontSize);
    ss += QString("QPushButton { background-color: %1; color: %2; border: 1px solid %3;"
                  " border-radius: 6px; font-family: \"%4\"; font-size: %5px; min-height: 44px; }\n")
            .arg(btnBg, fg, border, m_settings.buttonFont)
            .arg(m_settings.buttonFontSize);
    ss += QString("QPushButton:hover   { background-color: %1; }\n").arg(hover);
    ss += QString("QPushButton:pressed { background-color: %1; }\n").arg(pressed);
    ss += QString("QComboBox { background-color: %1; color: %2; border: 1px solid %3;"
                  " border-radius: 4px; padding: 4px; min-height: 24px; }\n").arg(btnBg, fg, border);
    ss += QString("QComboBox::drop-down { border: none; }\n");
    ss += QString("QComboBox QAbstractItemView { background-color: %1; color: %2;"
                  " border: 1px solid %3; selection-background-color: %4; }\n").arg(bg, fg, border, hover);
    ss += QString("QSpinBox { background-color: %1; color: %2; border: 1px solid %3;"
                  " border-radius: 4px; padding: 4px; min-height: 24px; }\n").arg(btnBg, fg, border);
    // Override button size for dialog button boxes so they don't inherit min-height: 44px
    ss += QString("QDialogButtonBox QPushButton"
                  " { min-height: 28px; padding: 4px 16px; font-size: 13px; }\n");
    return ss;
}

void Calculator::loadSettings() {
    QSettings qs("MyCalculator", "Calculator");
    m_settings.theme           = qs.value("theme",           "dark").toString();
    m_settings.displayFont     = qs.value("displayFont",     "Sans Serif").toString();
    m_settings.displayFontSize = qs.value("displayFontSize", 34).toInt();
    m_settings.buttonFont      = qs.value("buttonFont",      "Sans Serif").toString();
    m_settings.buttonFontSize  = qs.value("buttonFontSize",  17).toInt();
}

void Calculator::saveSettings() {
    QSettings qs("MyCalculator", "Calculator");
    qs.setValue("theme",           m_settings.theme);
    qs.setValue("displayFont",     m_settings.displayFont);
    qs.setValue("displayFontSize", m_settings.displayFontSize);
    qs.setValue("buttonFont",      m_settings.buttonFont);
    qs.setValue("buttonFontSize",  m_settings.buttonFontSize);
}
