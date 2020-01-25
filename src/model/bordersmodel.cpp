#include "bordersmodel.hpp"
#include "repository/skinrepository.hpp"

void BorderStorage::setStyle(WindowStyle* style)
{
    if (style) {
        auto bs = style->borderSet();
        for (int i = 0; i < BorderSet::count(); ++i) {
            auto bp = static_cast<Property::BorderPosition>(i);
            auto path = SkinRepository::instance().resolveFilename(bs.getBorder(bp).fileName());
            pixmaps[bp].load(path);
            observers[bp]->setPath(path);
        }
    } else {
        for (int i = 0; i < BorderSet::count(); ++i) {
            pixmaps[i] = QPixmap();
            observers[i]->setPath(QString());
        }
    }
}
