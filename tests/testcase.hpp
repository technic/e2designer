#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <QObject>
#include <QtTest>

class Runner
{
public:
    Runner();
    ~Runner();
    static int run(int argc, char** argv);
    static void add(QObject *test);
private:
    void addTest(QObject *test);
    int runTests(int argc, char** argv);
    static Runner &instance();
    QVector<QObject*> tests;
};

template<typename T>
class TestCase
{
public:
    TestCase() {
        Runner::add(new T());
    }
};

#endif // RUNNER_HPP
