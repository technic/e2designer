#include "pixmapstorage.hpp"

PixmapStorage::PixmapStorage(QObject* parent)
    : QObject(parent)
{
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &PixmapStorage::onFileChanged);
}

void PixmapStorage::registerObserver(const QString& path, PixmapWatcher* observer)
{
    Q_ASSERT(!m_observers.contains(path, observer));

    m_observers.insert(path, observer);
    m_watcher.addPath(path);
}

void PixmapStorage::unregisterObserver(const QString& path, PixmapWatcher* observer)
{
    Q_ASSERT(m_observers.contains(path, observer));

    m_observers.remove(path, observer);
    if (!m_observers.contains(path))
        m_watcher.removePath(path);
}

void PixmapStorage::onFileChanged(const QString& path)
{
    // Iterate over all values with the given key
    for (auto it = m_observers.find(path); it != m_observers.end() && it.key() == path; ++it) {
        it.value()->fileChangedEvent();
    }
}
