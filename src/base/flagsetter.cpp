#include "flagsetter.hpp"

FlagSetter::FlagSetter(bool* flag)
    : m_previous_value(*flag)
    , m_flag(flag)
{
    *m_flag = true;
}

FlagSetter::~FlagSetter()
{
    *m_flag = m_previous_value;
}
