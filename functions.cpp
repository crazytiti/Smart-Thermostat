#include "functions.h"

int dayofweek(unsigned long t)
{
    return ((t / 86400) + 4) % 7;
}
