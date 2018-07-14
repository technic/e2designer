#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("e2designer");
    QCoreApplication::setApplicationName("e2designer");
    QCoreApplication::setApplicationVersion("0.0.1"); // TODO: get from git tag
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "the skin.xml file to open.");
    parser.process(app);

    MainWindow window;
    if (!parser.positionalArguments().isEmpty())
        window.openFile(parser.positionalArguments().first());
    window.show();

    return app.exec();
}
