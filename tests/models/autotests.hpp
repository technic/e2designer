#ifndef AUTOTESTS_HPP
#define AUTOTESTS_HPP

#include <memory>
#include <QString>
#include <QList>
#include <QObject>
#include <QTest>
#include "base/singleton.hpp"

using TestFunc = std::function<int(int, char**)>;

class AutoTests : public SingletonMixin<AutoTests>
{
public:
    inline static void addTest(TestFunc f) { instance().addTestImpl(f); }
    static int run(int argc, char* argv[]);

private:
    void addTestImpl(TestFunc f);
    QList<TestFunc> m_list;
};

class Test
{
public:
    Test(TestFunc f) { AutoTests::addTest(f); }
};

#endif // AUTOTESTS_HPP
