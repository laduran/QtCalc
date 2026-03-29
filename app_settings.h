#pragma once
#include <QString>

struct AppSettings {
    QString theme           = QStringLiteral("dark");
    QString displayFont     = QStringLiteral("Sans Serif");
    int     displayFontSize = 34;
    QString buttonFont      = QStringLiteral("Sans Serif");
    int     buttonFontSize  = 17;
};
