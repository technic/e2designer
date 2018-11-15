#ifndef SKINREPOSITORY_H
#define SKINREPOSITORY_H

#include "base/singleton.hpp"
#include "model/colorsmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/fontsmodel.hpp"
#include "model/fontsmodel.hpp"
#include "model/screensmodel.hpp"
#include "model/windowstyle.hpp"
#include "repository/videooutputrepository.hpp"
#include <QDir>
#include <QObject>

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
    explicit SkinRepository(QObject* parent = Q_NULLPTR);

    // functions for easy access to singleton childs
    static ColorsModel* colors() { return instance().mColors; }
    static FontsModel* fonts() { return instance().mFonts; }
    static ScreensModel* screens() { return instance().mScreensModel; }
    static VideoOutputRepository* outputs() { return &instance().mOutputRepository; }
    static WindowStylesList *styles() { return &instance().mWindowStyles; }
    QSize outputSize() const;
    inline QDir dir() const { return mDirectory; }
    QString resolveFilename(const QString& path) const;

    bool loadFile(const QString& path);
    bool fromStream(QIODevice* device);
    bool save();

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;
    QString previewFilePath();

signals:

private slots:

private:
    ColorsModel* mColors;
    FontsModel* mFonts;
    ScreensModel* mScreensModel;
    VideoOutputRepository mOutputRepository;
    WindowStylesList mWindowStyles;
    QDir mDirectory;
};

#endif // SKINREPOSITORY_H
