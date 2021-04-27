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

template <> inline double qbswap<double>(double source)
{
    double result;
    char* s = (char*) &source;
    char* t = (char*) &result;
    t[0] = s[7];
    t[1] = s[6];
    t[2] = s[5];
    t[3] = s[4];
    t[4] = s[3];
    t[5] = s[2];
    t[6] = s[1];
    t[7] = s[0];
    return result;
}

#endif
