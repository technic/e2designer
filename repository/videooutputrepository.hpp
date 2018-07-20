#ifndef VIDEOOUTPUTREPOSITORY_H
#define VIDEOOUTPUTREPOSITORY_H

#include "repository/xmlnode.hpp"
#include <QVector>

class QXmlStreamReader;
class QXmlStreamWriter;

class VideoOutput : public XmlData
{
public:
    VideoOutput()
        : m_xres(0)
        , m_yres(0)
        , m_bpp(0)
        , m_id(-1)
    {
    }
    // Xml
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    int xres() { return m_xres; }
    int yres() { return m_yres; }
    int bpp() { return m_bpp; }
    int id() { return m_id; }

private:
    int m_xres;
    int m_yres;
    int m_bpp;
    int m_id;
};

class VideoOutputRepository : public XmlData
{
public:
    VideoOutputRepository();

    void addFromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    VideoOutput getOutput(int id = 0);
    void clear() { mOutputs.clear(); }

private:
    QVector<VideoOutput> mOutputs;
};

#endif // VIDEOOUTPUTREPOSITORY_H
