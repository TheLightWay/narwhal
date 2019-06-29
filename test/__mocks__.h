#ifndef TEST_MOCKS_H
#define TEST_MOCKS_H

#include "dummy_functions.h"

/*
NARMOCK_DECLARATION add
NARMOCK_LINKER_FLAGS -Wl,--wrap=add
*/

typedef struct _narmock_parameters_add
{
    int arg1;
    int arg2;
} _narmock_parameters_add;

typedef struct _narmock_state_type_add _narmock_state_type_add;

struct _narmock_state_type_add
{
    _narmock_state_type_add *(*mock_return)(int return_value);
    _narmock_state_type_add *(*mock_implementation)(int (*implementation)(int arg1, int arg2));
    _narmock_state_type_add *(*disable_mock)(void);
};

_narmock_state_type_add *_narwhal_mock_add();

/*
NARMOCK_DECLARATION output_message
NARMOCK_LINKER_FLAGS -Wl,--wrap=output_message
*/

typedef struct _narmock_parameters_output_message
{
    char *arg1;
} _narmock_parameters_output_message;

typedef struct _narmock_state_type_output_message _narmock_state_type_output_message;

struct _narmock_state_type_output_message
{
    _narmock_state_type_output_message *(*mock_return)(void);
    _narmock_state_type_output_message *(*mock_implementation)(void (*implementation)(char *arg1));
    _narmock_state_type_output_message *(*disable_mock)(void);
};

_narmock_state_type_output_message *_narwhal_mock_output_message();

#endif
