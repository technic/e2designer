#include "autotests.hpp"

int AutoTests::run(int argc, char* argv[])
{
    int ret = 0;
    for (TestFunc& f : instance().m_list) {
        ret += f(argc, argv);
    }
    return ret;
}

void AutoTests::addTestImpl(TestFunc f)
{
    m_list.append(f);
}

int main(int argc, char* argv[])
{
    return AutoTests::run(argc, argv);
}
