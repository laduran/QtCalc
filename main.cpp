#include <QApplication>
#include <QCommandLineParser>
#include "calculator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Calculator");

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple calculator");
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("theme", "Override theme for this session (dark|light).", "theme"));
    parser.process(app);

    QString themeOverride;
    if (parser.isSet("theme")) {
        const QString val = parser.value("theme").toLower();
        if (val == "dark" || val == "light")
            themeOverride = val;
    }

    Calculator calc(themeOverride);
    calc.setWindowTitle("Calculator");
    calc.show();
    return app.exec();
}
