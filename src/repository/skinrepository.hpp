#pragma once

#include "base/singleton.hpp"
#include "model/outputsmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/fontsmodel.hpp"
#include "model/fontsmodel.hpp"
#include "model/screensmodel.hpp"
#include "model/windowstyle.hpp"
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
    static ColorsModel* colors() { return instance().m_colors; }
    static FontsModel* fonts() { return instance().m_fonts; }
    static ScreensModel* screens() { return instance().m_screensModel; }
    static OutputsModel* outputs() { return &instance().m_outputRepository; }
    static WindowStylesList* styles() { return &instance().m_windowStyles; }
    QSize outputSize() const;
    inline QDir dir() const { return m_directory; }
    QString resolveFilename(const QString& path) const;

    bool open(const QString& path);
    bool save();
    bool saveAs(const QString& path);
    bool create(const QString& path);
    bool isOpened() const;

    bool fromXmlDocument(QXmlStreamReader& xml);
    void fromXml(QXmlStreamReader& xml);
    void toXml(XmlStreamWriter& xml) const;
    QString skinFilePath();
    QString previewFilePath();

    QString lastError() const { return m_errorMessage; }

signals:
    void filePathChanged(const QString& path);

private slots:

private:
    ColorsModel* m_colors;
    ColorRolesModel m_roles;
    FontsModel* m_fonts;
    ScreensModel* m_screensModel;
    OutputsModel m_outputRepository;
    WindowStylesList m_windowStyles;
    WindowStyle defaultStyle;
    QDir m_directory;

    // Error handling
    bool setError(const QString& message);
    QString m_errorMessage;

    void clear();
};
