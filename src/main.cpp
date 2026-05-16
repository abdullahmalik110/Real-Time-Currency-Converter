#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Currency Converter");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("MyApp");

    // Fusion = clean modern look on all platforms
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
