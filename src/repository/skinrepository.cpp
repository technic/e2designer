#include "skinrepository.hpp"
#include <QDebug>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "base/xmlstreamwriter.hpp"

SkinRepository::SkinRepository(QObject* parent)
    : QObject(parent)
    , m_colors(new ColorsModel(this))
    , m_roles(*m_colors)
    , m_fonts(new FontsModel(this))
    , m_screensModel(new ScreensModel(*m_colors, m_roles, *m_fonts, this))
{}

QSize SkinRepository::outputSize() const
{
    return m_outputRepository.getOutput(0).size();
}

QString SkinRepository::resolveFilename(const QString& path) const
{
    if (path.isEmpty()) {
        return QString();
    }
    QDir dir = this->dir();
    // First assume that file prefix is our directory name
    dir.cdUp();
    if (dir.exists(path)) {
        return dir.filePath(path);
    } else {
        // Secondly replace file prefix with our directory name
        QStringList list = path.split("/");
        list.pop_front();
        dir = this->dir();
        if (dir.exists(list.join("/"))) {
            return dir.filePath(list.join("/"));
        }
    }
    return QString();
}

bool SkinRepository::open(const QString& path)
{
    m_directory = QDir(path);
    if (!m_directory.exists()) {
        return setError(tr("Directory does not exists"));
    }

    m_screensModel->loadPreviews(previewFilePath());

    QString skinFile = m_directory.filePath("skin.xml");
    QFile file(skinFile);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        return setError(file.errorString());
    }
    emit filePathChanged(skinFile);

    QXmlStreamReader xml(&file);
    ok = fromXmlDocument(xml);
    if (!ok) {
        return setError(xml.errorString());
    }
    // Check for read errors
    if (file.error() != QFileDevice::FileError::NoError) {
        return setError(file.errorString());
    }
    file.close();

    return ok;
}

bool SkinRepository::fromXmlDocument(QXmlStreamReader& xml)
{
    while (!xml.atEnd()) {
        xml.readNext();
        switch (xml.tokenType()) {
        case QXmlStreamReader::StartElement:
            if (xml.name() == "skin") {
                fromXml(xml);
            } else {
                xml.raiseError(tr("Unknow tag in skin: ") + xml.name());
            }
            break;
        default:
            break;
        }
    }
    // FIXME: should we call setError() here?
    return !xml.hasError();
}

bool SkinRepository::save()
{
    if (!isOpened()) {
        setError(tr("Skin directory is not specified"));
        return false;
    }

    QFile file(m_directory.filePath("skin.xml"));
    bool ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        setError(file.errorString());
        return false;
    }

    XmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    toXml(xml);
    xml.writeEndDocument();

    file.close();
    m_screensModel->savePreviewTree(previewFilePath());

    // Tell undo model that the state is saved
    m_screensModel->undoStack()->setClean();
    return true;
}

bool SkinRepository::saveAs(const QString& path)
{
    QDir oldDir = m_directory;
    m_directory = QDir(path);
    qDebug() << oldDir.absolutePath() << m_directory.absolutePath();
    if (oldDir == m_directory || QFileInfo(m_directory, "skin.xml").exists()
        || QFileInfo(m_directory, "preview.xml").exists()) {
        return setError("This folder already contains a skin");
    }
    bool saved = save();
    if (!saved) {
        m_directory = oldDir;
    }
    return saved;
}

bool SkinRepository::isOpened() const
{
    // This is default path
    return m_directory.path() != ".";
}

void SkinRepository::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "skin");

    m_screensModel->clear();
    m_outputRepository.clear();
    m_roles.setStyle(nullptr);
    m_windowStyles.clear();
    m_colors->removeRows(0, m_colors->rowCount());
    m_fonts->removeRows(0, m_fonts->rowCount());

    while (nextXmlChild(xml)) {
        qDebug() << xml.tokenString() << xml.name() << xml.text() << xml.attributes().value("name");

        if (xml.isStartElement()) {
            if (xml.name() == "output") {
                m_outputRepository.addFromXml(xml);

            } else if (xml.name() == "windowstyle") {
                WindowStyle style;
                style.fromXml(xml);
                m_windowStyles.append(style);

            } else if (xml.name() == "colors") {
                m_colors->fromXml(xml);

            } else if (xml.name() == "fonts") {
                m_fonts->fromXml(xml);

            } else if (xml.name() == "screen") {
                m_screensModel->appendFromXml(xml);

            } else {
                qWarning() << "Unknown tag" << xml.name();
                xml.skipCurrentElement();
            }
        }
    }

    if (m_windowStyles.itemsCount() > 0) {
        defaultStyle = m_windowStyles.itemAt(0);
        m_roles.setStyle(&defaultStyle);
    }
}

void SkinRepository::toXml(XmlStreamWriter& xml) const
{
    xml.writeStartElement("skin");
    m_outputRepository.toXml(xml);
    m_windowStyles.toXml(xml);
    m_fonts->toXml(xml);
    m_colors->toXml(xml);
    m_screensModel->toXml(xml);
    xml.writeEndElement();
}

QString SkinRepository::previewFilePath()
{
    return dir().filePath("preview.xml");
}

/**
 * @brief Save error message
 * @param message that can be displayed to the user
 * @return always returns false, for use with return statement
 */
bool SkinRepository::setError(const QString& message)
{
    m_errorMessage = message;
    qWarning() << "Error occured:" << message;
    return false;
}
