#include "skinrepository.hpp"
#include <QDebug>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

SkinRepository::SkinRepository(QObject* parent)
    : QObject(parent)
    , mColors(new ColorsModel(this))
    , mFonts(new FontsModel(this))
    , mScreensModel(new ScreensModel(this))
{
}

QSize SkinRepository::outputSize() const
{
    return mOutputRepository.getOutput(0).size();
}

// SkinRepository& SkinRepository::instance()
//{
//	static SkinRepository self;
//	return self;
//}

QString SkinRepository::resolveFilename(const QString& path) const
{
    if (path.isEmpty()) {
        return QString();
    }
    QDir dir;
    dir = this->dir();
    // First assume that file prefix is a our directory name
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

bool SkinRepository::loadFile(const QString& path)
{
    mDirectory = QDir(path);
    if (!mDirectory.exists()) {
        return false;
    }

    mScreensModel->loadPreviews();

    QFile file(mDirectory.filePath("skin.xml"));
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
        return false;
    ok = fromStream(&file);
    file.close();

    return ok;
}

bool SkinRepository::fromStream(QIODevice* device)
{
    QXmlStreamReader xml(device);

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
    qDebug() << xml.errorString();
    Q_ASSERT(!xml.hasError());
    return true; // FIXME
}

bool SkinRepository::save()
{
    // FIXME: implement!
    QFile file(mDirectory.filePath("skin.xml"));
    bool ok = file.open(QIODevice::WriteOnly);
    if (!ok)
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    toXml(xml);

    file.close();
    return true;
}

void SkinRepository::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "skin");

    mScreensModel->clear();
    mOutputRepository.clear();

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
