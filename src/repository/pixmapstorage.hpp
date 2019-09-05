#pragma once

#include <QObject>
#include <QPixmapCache>
#include <QFileSystemWatcher>
#include "base/singleton.hpp"

class PixmapWatcher;

/**
 * @brief Collection of png files used by skin
 * Provides interface to get Pixmap by filename
 * Watches file system changes and notifies about changed Pixmaps
 */
class PixmapStorage : public SingletonMixin<PixmapStorage>, public QObject
{
    Q_DISABLE_COPY(PixmapStorage)
public:
    explicit PixmapStorage(QObject* parent = Q_NULLPTR);
    void registerObserver(const QString& path, PixmapWatcher* observer);
    void unregisterObserver(const QString& path, PixmapWatcher* observer);

private slots:
    /// Route QFileSystemWatcher event to registered observers
    void onFileChanged(const QString& path);

private:
    QFileSystemWatcher m_watcher;
    QMultiHash<QString, PixmapWatcher*> m_observers;
};

class PixmapWatcher
{
    friend class PixmapStorage;
    Q_DISABLE_COPY(PixmapWatcher)
public:
    PixmapWatcher() {}
    explicit PixmapWatcher(const QString& path) { setPath(path); }

    QString path() const { return m_path; }
    void setPath(const QString& path)
    {
        auto& storage = PixmapStorage::instance();
        if (!m_path.isNull()) {
            storage.unregisterObserver(m_path, this);
        }
        m_path = path;
        storage.registerObserver(m_path, this);
    }
    ~PixmapWatcher() { PixmapStorage::instance().unregisterObserver(m_path, this); }

protected:
    /// Override this function to handle file change events
    virtual void fileChangedEvent() = 0;

private:
    QString m_path;
};

