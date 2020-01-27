#pragma once

#include "repository/pixmapstorage.hpp"
#include "model/windowstyle.hpp"
#include <memory>
#include <vector>

class BorderStorage : public QObject
{
    Q_OBJECT
public:
    BorderStorage()
    {
        for (int i = 0; i < BorderSet::count(); ++i) {
            pixmaps.append(QPixmap());
            observers.push_back(std::make_unique<Observer>(this, i));
        }
    }

    void setStyle(WindowStyle* style);

    const QPixmap& getPixmap(int i) const { return pixmaps[i]; }
    const QPixmap& operator[](int i) const { return getPixmap(i); }

signals:
    void changed();

private:
    class Observer : public PixmapWatcher
    {
    public:
        Observer(BorderStorage* s, int i, QString path = QString())
            : PixmapWatcher(path)
            , storage(s)
            , index(i)
        {}
        virtual ~Observer() = default;

    protected:
        void fileChangedEvent() final { storage->reload(index, path()); }

    private:
        BorderStorage* storage;
        int index;
    };

    void reload(int index, const QString& path)
    {
        pixmaps[index].load(path);
        emit changed();
    }

    QVector<QPixmap> pixmaps;
    std::vector<std::unique_ptr<Observer>> observers;
};
