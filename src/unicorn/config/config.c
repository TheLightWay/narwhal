#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "unicorn/config/config.h"


/*
 * Global config
 */

UnicornConfig _unicorn_config =
{
    .fast_session = false
};


/*
 * Load configuration
 */

void unicorn_load_config()
{
    char *session = getenv("UNICORN_SESSION");

    if (session != NULL)
    {
        _unicorn_config.fast_session = strcmp(session, "fast") == 0;
    }
}
