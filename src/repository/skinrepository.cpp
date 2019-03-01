#include "skinrepository.hpp"
#include <QDebug>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

SkinRepository::SkinRepository(QObject* parent)
    : QObject(parent)
    , mColors(new ColorsModel(this))
    , mRoles(*mColors)
    , mFonts(new FontsModel(this))
    , mScreensModel(new ScreensModel(*mColors, mRoles, *mFonts, this))
{}

QSize SkinRepository::outputSize() const
{
    return mOutputRepository.getOutput(0).size();
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
    mDirectory = QDir(path);
    if (!mDirectory.exists()) {
        return setError(tr("Directory does not exists"));
    }

    mScreensModel->loadPreviews(previewFilePath());

    QString skinFile = mDirectory.filePath("skin.xml");
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

    QFile file(mDirectory.filePath("skin.xml"));
    bool ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        setError(file.errorString());
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    toXml(xml);
    xml.writeEndDocument();

    file.close();
    mScreensModel->savePreviewTree(previewFilePath());
    return true;
}

bool SkinRepository::saveAs(const QString& path)
{
    QDir oldDir = mDirectory;
    mDirectory = QDir(path);
    qDebug() << oldDir.absolutePath() << mDirectory.absolutePath();
    if (oldDir == mDirectory || QFileInfo(mDirectory, "skin.xml").exists()
        || QFileInfo(mDirectory, "preview.xml").exists()) {
        return setError("This folder already contains a skin");
    }
    bool saved = save();
    if (!saved) {
        mDirectory = oldDir;
    }
    return saved;
}

bool SkinRepository::isOpened() const
{
    // This is default path
    return mDirectory.path() != ".";
}

void SkinRepository::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "skin");

    mScreensModel->clear();
    mOutputRepository.clear();
    mRoles.setStlye(nullptr);
    mWindowStyles.clear();
    mColors->removeRows(0, mColors->rowCount());
    mFonts->removeRows(0, mFonts->rowCount());

    while (nextXmlChild(xml)) {
        qDebug() << xml.tokenString() << xml.name() << xml.text() << xml.attributes().value("name");

        if (xml.isStartElement()) {
            if (xml.name() == "output") {
                mOutputRepository.addFromXml(xml);

            } else if (xml.name() == "windowstyle") {
                WindowStyle style;
                style.fromXml(xml);
                mWindowStyles.append(style);

            } else if (xml.name() == "colors") {
                mColors->fromXml(xml);

            } else if (xml.name() == "fonts") {
                mFonts->fromXml(xml);

            } else if (xml.name() == "screen") {
                mScreensModel->appendFromXml(xml);

            } else {
                qWarning() << "Unknown tag" << xml.name();
                xml.skipCurrentElement();
            }
        }
    }

    if (mWindowStyles.itemsCount() > 0) {
        defaultStyle = mWindowStyles.itemAt(0);
        mRoles.setStlye(&defaultStyle);
    }
}

void SkinRepository::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("skin");
    mOutputRepository.toXml(xml);
    mWindowStyles.toXml(xml);
    mFonts->toXml(xml);
    mColors->toXml(xml);
    mScreensModel->toXml(xml);
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
    mErrorMessage = message;
    qWarning() << "Error occured:" << message;
    return false;
}
