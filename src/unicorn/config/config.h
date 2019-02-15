#ifndef UNICORN_CONFIG_H
#define UNICORN_CONFIG_H


#include <stdbool.h>

#include "unicorn/types.h"


extern UnicornConfig _unicorn_config;


struct UnicornConfig
{
    bool fast_session;
};

void unicorn_load_config();


#endif
