#ifndef PIXMAPSTORAGE_H
#define PIXMAPSTORAGE_H

#include <QObject>
#include <QPixmapCache>

/**
 * @brief Collection of png files used by skin
 * Provides interface to get Pixmap by filename
 * Watches file system changes and notifies about changed Pixmaps
 */
class PixmapStorage : public QObject
{
    Q_OBJECT
public:
    explicit PixmapStorage(QObject* parent = Q_NULLPTR);

    // FIXME: implement me!

    void load(const QString& file);
    void release(const QString& file);
    QPixmap find(const QString& file);

signals:
    void pixmapChanged(const QString& name);

public slots:

private:
};

#endif // PIXMAPSTORAGE_H
