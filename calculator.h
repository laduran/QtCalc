#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include "app_settings.h"

class Calculator : public QMainWindow {
    Q_OBJECT

public:
    explicit Calculator(const QString &themeOverride = {}, QWidget *parent = nullptr);

private:
    void digitPressed(const QString &digit);
    void operatorPressed(const QString &op);
    void equalsPressed();
    void decimalPressed();
    void clearEntry();
    void allClear();

    void    openSettings();
    void    applySettings();
    QString buildStyleSheet() const;
    QString formatResult(double value) const;
    void    loadSettings();
    void    saveSettings();

    QLineEdit  *m_display;
    double      m_pendingValue      = 0.0;
    QString     m_pendingOp;
    bool        m_waitingForOperand = true;
    AppSettings m_settings;
};
