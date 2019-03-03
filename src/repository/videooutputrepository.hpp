#ifndef VIDEOOUTPUTREPOSITORY_H
#define VIDEOOUTPUTREPOSITORY_H

#include "repository/xmlnode.hpp"
#include "model/namedlist.hpp"
#include <QVector>
#include <QSize>

class QXmlStreamReader;
class QXmlStreamWriter;

struct VideoOutputData
{
    QSize resolution;
    int bpp;
};

class VideoOutput : public XmlData
{
public:
    VideoOutput()
        : m_xres(0)
        , m_yres(0)
        , m_bpp(0)
        , m_id(-1)
    {}
    VideoOutput(const QString& id, const VideoOutputData& data);

    // Xml
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    int xres() const { return m_xres; }
    int yres() const { return m_yres; }
    QSize size() const { return QSize(m_xres, m_yres); }
    int bpp() const { return m_bpp; }
    int id() const { return m_id; }

    typedef VideoOutputData Value;
    VideoOutputData value() const { return VideoOutputData{ size(), m_bpp }; }
    QString name() const { return QString::number(m_id); }
    bool isNull() const { return m_id == -1; }

private:
    int m_xres;
    int m_yres;
    int m_bpp;
    int m_id;
};

class VideoOutputRepository : public QObject, public NamedList<VideoOutput>, public XmlData
{
    Q_OBJECT
public:
    VideoOutputRepository();

    void addFromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    inline VideoOutput getOutput(int id = 0) const { return getValue(QString::number(id)); }
    void clear() { removeItems(0, itemsCount()); }

signals:
    void valueChanged(int id, const VideoOutput& output) const;

protected:
    bool insertRows(int pos, int count);
    void emitValueChanged(const QString& name, const VideoOutput& value) const final;
};

#endif // VIDEOOUTPUTREPOSITORY_H
