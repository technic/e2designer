#ifndef ATTRITEMFACTORY_H
#define ATTRITEMFACTORY_H

#include "attr/attritem.hpp"
#include "base/tree.hpp"
#include "base/singleton.hpp"

class WidgetData;
class AttrItem;
class AttrItemCreatorBase;

/**
 * @brief Creates AttrItem implementation suitable for given attr type
 */
class AttrItemFactory: public SingletonMixin<AttrItemFactory>
{
public:
    inline static void registerAdapter(int type, AttrItemCreatorBase *creator) {
        instance().doRegisterAdapter(type, creator);
    }
    inline static AttrItem* createAdapter(WidgetData* widget, int attrType, int attrKey) {
        return instance().doCreateAdapter(widget, attrType, attrKey);
    }
    ~AttrItemFactory();

private:
    void doRegisterAdapter(int type, AttrItemCreatorBase *creator);
    AttrItem* doCreateAdapter(WidgetData* widget, int attrType, int attrKey);
    // own
    QHash<int, AttrItemCreatorBase*> mCreatorsMap;
};

class AttrItemCreatorBase
{
public:
    virtual AttrItem* createAdapter(WidgetData *widget, int attrKey) = 0;
    virtual ~AttrItemCreatorBase() { }
};

template<typename T>
class AttrItemCreator : public AttrItemCreatorBase
{
public:
    AttrItem* createAdapter(WidgetData *widget, int attrKey) final {
        return new T(widget, attrKey);
    }
};

template<typename T, typename I>
class AttrItemRegistrator
{
public:
    AttrItemRegistrator() {
        AttrItemFactory::registerAdapter(qMetaTypeId<T>(), new AttrItemCreator<I>());
    }
    static AttrItemRegistrator registrator;
};

#endif // ATTRITEMFACTORY_H
