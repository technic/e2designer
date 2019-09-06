#pragma once

#include "namedlist.hpp"
#include "movablelistmodel.hpp"
#include "repository/xmlnode.hpp"
#include <QAbstractTableModel>
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

    using Value = VideoOutputData;
    VideoOutputData value() const { return VideoOutputData{ size(), m_bpp }; }
    QString name() const { return QString::number(m_id); }
    bool isNull() const { return m_id == -1; }

private:
    int m_xres;
    int m_yres;
    int m_bpp;
    int m_id;
};

class VideoOutputRepository : public NamedList<VideoOutput>, public XmlData
{
public:
    VideoOutputRepository();

    void appendFromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    inline VideoOutput getOutput(int id = 0) const { return getValue(QString::number(id)); }
};

class OutputsModel : public MovableListModel, public VideoOutputRepository
{
    Q_OBJECT

public:
    explicit OutputsModel(QObject* parent = nullptr);

    enum
    {
        ColumnId,
        ColumnXRes,
        ColumnYRes,
        ColumnBpp,
        ColumnsCount,
    };

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insert(int row, const VideoOutput& item);
    bool append(const VideoOutput& output) { return insert(itemsCount(), output); }
    // bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    void clear() { removeItems(0, itemsCount()); }

    bool moveRows(const QModelIndex& sourceParent,
                  int sourceRow,
                  int count,
                  const QModelIndex& destinationParent,
                  int destinationChild) override;

signals:
    void valueChanged(int id, const VideoOutput& output) const;

protected:
    void emitValueChanged(const QString& name, const VideoOutput& value) const final;
};
