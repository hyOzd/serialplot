#include <QtGlobal>

#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))

template <> inline float qbswap<float>(float source)
{
    float result;
    char* s = (char*) &source;
    char* t = (char*) &result;
    t[0] = s[3];
    t[1] = s[2];
    t[2] = s[1];
    t[3] = s[0];
    return result;
}

#endif
