#ifndef OUTPUTSMODEL_HPP
#define OUTPUTSMODEL_HPP
#include "repository/videooutputrepository.hpp"
#include "model/movablelistmodel.hpp"
#include <QAbstractTableModel>

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

#endif // OUTPUTSMODEL_HPP
