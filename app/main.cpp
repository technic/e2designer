#include "mainwindow.hpp"
#include "gitversion.hpp"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("e2designer");
    QCoreApplication::setApplicationName("e2designer");
    QCoreApplication::setApplicationVersion(Git::version);
    app.setWindowIcon(QIcon(":/icons/misc/e2designer.png"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "the skin.xml file to open.");
    parser.process(app);

    Q_INIT_RESOURCE(resources);
    MainWindow window;
    if (!parser.positionalArguments().isEmpty())
        window.openFile(parser.positionalArguments().first());
    window.show();

    return app.exec();
}
