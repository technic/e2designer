#include "testcase.hpp"

//Runner Runner::runner;

Runner::Runner()
{
}

Runner::~Runner()
{
    qDeleteAll(tests);
}

int Runner::run(int argc, char **argv)
{
    return instance().runTests(argc, argv);
}

void Runner::add(QObject *test)
{
    instance().addTest(test);
}

void Runner::addTest(QObject *test)
{
    tests.append(test);
}

int Runner::runTests(int argc, char **argv)
{
    int status = 0;
    for (QObject *obj : tests) {
        status |= QTest::qExec(obj, argc, argv);
    }
    return status;
}

Runner &Runner::instance() {
    static Runner self;
    return self;
}
