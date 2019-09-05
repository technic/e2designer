#pragma once

#include <QtGlobal>

/**
 * @brief keep bool flag true within current scope
 * use RAII to keep flag true while object alive
 * and set to orginal value when object goes out of scope
 */

class FlagSetter
{
    Q_DISABLE_COPY(FlagSetter)

public:
    FlagSetter(bool* flag);
    ~FlagSetter();

private:
    bool m_previous_value;
    bool* m_flag;
};
