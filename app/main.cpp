#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("Pixel Studio"));
    QCoreApplication::setOrganizationName(QStringLiteral("PixelStudio"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("image"), QStringLiteral("Optional image to open on startup."));
    parser.process(application);

    MainWindow window;
    window.show();
    if (!parser.positionalArguments().isEmpty()) {
        const QString path = QFileInfo(parser.positionalArguments().first()).absoluteFilePath();
        window.openImage(path);
    }
    return application.exec();
}