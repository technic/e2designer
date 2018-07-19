#ifndef SKINREPOSITORY_H
#define SKINREPOSITORY_H

#include <QObject>
#include <QDir>
#include "base/singleton.h"
#include "model/colorsmodel.h"
#include "model/fontsmodel.h"
#include "model/screensmodel.h"
#include "repository/videooutputrepository.h"
#include "model/colorsmodel.h"
#include "model/fontsmodel.h"
#include "model/windowstyle.h"

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * @brief provides storage of the skin data
 * stores C++ types like int, enum, QPoint, QSize, struct
 */
class SkinRepository : public QObject, public SingletonMixin<SkinRepository>, public XmlData
{
    Q_OBJECT
    Q_DISABLE_COPY(SkinRepository)

public:
    explicit SkinRepository(QObject *parent = Q_NULLPTR);

    // functions for easy access to singleton childs
    static ColorsModel* colors() { return instance().mColors; }
    static FontsModel* fonts() { return instance().mFonts; }
    static ScreensModel* screens() { return instance().mScreensModel; }

    QSize outputSize() { return QSize(1920, 1080); } // TODO: implement

    inline QDir dir() const { return mDirectory; }
    QString resolveFilename(const QString &path) const;

    bool loadFile(const QString &path);
    bool fromStream(QIODevice *device);
    bool save();

    void fromXml(QXmlStreamReader &xml);
    void toXml(QXmlStreamWriter &xml) const;

signals:

private slots:

private:
    ColorsModel *mColors;
    FontsModel *mFonts;
    ScreensModel *mScreensModel;
    VideoOutputRepository mOutputRepository;
    WindowStylesList mWindowStyles;
    QDir mDirectory;
};

#endif // SKINREPOSITORY_H
