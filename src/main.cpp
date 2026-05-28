// ─────────────────────────────────────────────────────────────────────────────
// main.cpp  —  Entry point ONLY.
//
// Per project requirements: main.cpp contains ONLY the main() function.
// All classes and logic live in the custom library (src/ files).
// ─────────────────────────────────────────────────────────────────────────────

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Currency Converter");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("UET OOP Project");
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
