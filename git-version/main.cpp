#include <QCoreApplication>
#include <QCommandLineParser>
#include <QProcess>
#include <QDebug>
#include <QFile>

bool writeFile(const QString& filename, const QString &version)
{
    QFile f(filename);
    bool ok = f.open(QIODevice::WriteOnly);
    if (!ok) {
        qWarning() << "Failed to open file" << filename;
        return false;
    }
    QTextStream output(&f);
    output << "#include \"gitversion.hpp\"" << "\n";
    output << QString("const char* Git::version = \"%1\"").arg(version) << ";" << "\n";
    f.close();
    if (f.error() != QFileDevice::NoError) {
        qWarning() << "Error:" << f.errorString();
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString outputFile = "gitversion.cpp";

    QCommandLineParser parser;
    parser.setApplicationDescription("Get version from git and write into a cpp file.");
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("C", "Working directory", "directory"));
    parser.addPositionalArgument("cppfile",
        QString("Destination file to create, default value is %1").arg(outputFile), "[file]");
    parser.process(app);

    if (!parser.positionalArguments().isEmpty()) {
        outputFile = parser.positionalArguments().at(0);
    }

    QProcess p;
    auto arguments = QStringList();
    if (parser.isSet("C")) {
        arguments << "-C" << parser.value("C");
    }
    arguments << "describe" << "--tags" << "--always";
    p.start("git", arguments, QIODevice::ReadOnly);
    if (p.waitForFinished(10000)) {
        if (p.exitCode() != 0) {
            qWarning() << "Git process terminated with error:\n" << p.readAllStandardError();
            return 2;
        }
        QByteArray data = p.readAllStandardOutput();
        QString version = QString(data).trimmed();
        qDebug() << "Detected version:" << version;
        bool ok = writeFile(outputFile, version);
        return ok ? 0 : 1;
    } else {
        qWarning() << "Failed to wait for git process";
        return 2;
    }
}
