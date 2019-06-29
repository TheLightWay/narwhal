/*
NARMOCK_DECLARATIONS_BEGIN
*/

#include <time.h>

#include "dummy_functions.h"

/*
NARMOCK_DECLARATION time
NARMOCK_LINKER_FLAGS -Wl,--wrap=time
*/

typedef struct _narmock_parameters_time
{
    time_t *arg1;
} _narmock_parameters_time;

typedef struct _narmock_state_type_time _narmock_state_type_time;

struct _narmock_state_type_time
{
    _narmock_state_type_time *(*mock_return)(time_t return_value);
    _narmock_state_type_time *(*mock_implementation)(time_t (*implementation)(time_t *arg1));
    _narmock_state_type_time *(*disable_mock)(void);
};

_narmock_state_type_time *_narwhal_mock_time();

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

/*
NARMOCK_DECLARATIONS_END
*/

/*
NARMOCK_IMPLEMENTATION time
*/

time_t __real_time(time_t *arg1);

typedef struct _narmock_state_private_type_time _narmock_state_private_type_time;

struct _narmock_state_private_type_time
{
    _narmock_state_type_time public;

    int state;
    time_t return_value;
    time_t (*implementation)(time_t *arg1);
};

_narmock_state_type_time *_narmock_function_mock_return_time(time_t return_value);
_narmock_state_type_time *_narmock_function_mock_implementation_time(time_t (*implementation)(time_t *arg1));
_narmock_state_type_time *_narmock_function_disable_mock_time();

_narmock_state_private_type_time _narmock_state_global_time =
{
    .public = {
        .mock_return = _narmock_function_mock_return_time,
        .mock_implementation = _narmock_function_mock_implementation_time,
        .disable_mock = _narmock_function_disable_mock_time
    },

    .state = 0
};

time_t __wrap_time(time_t *arg1)
{
    switch (_narmock_state_global_time.state)
    {
        case 1:
            return _narmock_state_global_time.return_value;
        case 2:
            return _narmock_state_global_time.implementation(arg1);
        default:
            return __real_time(arg1);
    }
}

_narmock_state_type_time *_narmock_function_mock_return_time(time_t return_value)
{
    _narmock_state_global_time.state = 1;
    _narmock_state_global_time.return_value = return_value;

    return &_narmock_state_global_time.public;
}

_narmock_state_type_time *_narmock_function_mock_implementation_time(time_t (*implementation)(time_t *arg1))
{
    _narmock_state_global_time.state = 2;
    _narmock_state_global_time.implementation = implementation;

    return &_narmock_state_global_time.public;
}

_narmock_state_type_time *_narmock_function_disable_mock_time()
{
    _narmock_state_global_time.state = 0;

    return &_narmock_state_global_time.public;
}

_narmock_state_type_time *_narwhal_mock_time()
{
    return &_narmock_state_global_time.public;
}

/*
NARMOCK_IMPLEMENTATION add
*/

int __real_add(int arg1, int arg2);

typedef struct _narmock_state_private_type_add _narmock_state_private_type_add;

struct _narmock_state_private_type_add
{
    _narmock_state_type_add public;

    int state;
    int return_value;
    int (*implementation)(int arg1, int arg2);
};

_narmock_state_type_add *_narmock_function_mock_return_add(int return_value);
_narmock_state_type_add *_narmock_function_mock_implementation_add(int (*implementation)(int arg1, int arg2));
_narmock_state_type_add *_narmock_function_disable_mock_add();

_narmock_state_private_type_add _narmock_state_global_add =
{
    .public = {
        .mock_return = _narmock_function_mock_return_add,
        .mock_implementation = _narmock_function_mock_implementation_add,
        .disable_mock = _narmock_function_disable_mock_add
    },

    .state = 0
};

int __wrap_add(int arg1, int arg2)
{
    switch (_narmock_state_global_add.state)
    {
        case 1:
            return _narmock_state_global_add.return_value;
        case 2:
            return _narmock_state_global_add.implementation(arg1, arg2);
        default:
            return __real_add(arg1, arg2);
    }
}

_narmock_state_type_add *_narmock_function_mock_return_add(int return_value)
{
    _narmock_state_global_add.state = 1;
    _narmock_state_global_add.return_value = return_value;

    return &_narmock_state_global_add.public;
}

_narmock_state_type_add *_narmock_function_mock_implementation_add(int (*implementation)(int arg1, int arg2))
{
    _narmock_state_global_add.state = 2;
    _narmock_state_global_add.implementation = implementation;

    return &_narmock_state_global_add.public;
}

_narmock_state_type_add *_narmock_function_disable_mock_add()
{
    _narmock_state_global_add.state = 0;

    return &_narmock_state_global_add.public;
}

_narmock_state_type_add *_narwhal_mock_add()
{
    return &_narmock_state_global_add.public;
}

/*
NARMOCK_IMPLEMENTATION output_message
*/

void __real_output_message(char *arg1);

typedef struct _narmock_state_private_type_output_message _narmock_state_private_type_output_message;

struct _narmock_state_private_type_output_message
{
    _narmock_state_type_output_message public;

    int state;
    
    void (*implementation)(char *arg1);
};

_narmock_state_type_output_message *_narmock_function_mock_return_output_message(void);
_narmock_state_type_output_message *_narmock_function_mock_implementation_output_message(void (*implementation)(char *arg1));
_narmock_state_type_output_message *_narmock_function_disable_mock_output_message();

_narmock_state_private_type_output_message _narmock_state_global_output_message =
{
    .public = {
        .mock_return = _narmock_function_mock_return_output_message,
        .mock_implementation = _narmock_function_mock_implementation_output_message,
        .disable_mock = _narmock_function_disable_mock_output_message
    },

    .state = 0
};

void __wrap_output_message(char *arg1)
{
    switch (_narmock_state_global_output_message.state)
    {
        case 1:
            return ;
        case 2:
            return _narmock_state_global_output_message.implementation(arg1);
        default:
            return __real_output_message(arg1);
    }
}

_narmock_state_type_output_message *_narmock_function_mock_return_output_message(void)
{
    _narmock_state_global_output_message.state = 1;
    

    return &_narmock_state_global_output_message.public;
}

_narmock_state_type_output_message *_narmock_function_mock_implementation_output_message(void (*implementation)(char *arg1))
{
    _narmock_state_global_output_message.state = 2;
    _narmock_state_global_output_message.implementation = implementation;

    return &_narmock_state_global_output_message.public;
}

_narmock_state_type_output_message *_narmock_function_disable_mock_output_message()
{
    _narmock_state_global_output_message.state = 0;

    return &_narmock_state_global_output_message.public;
}

_narmock_state_type_output_message *_narwhal_mock_output_message()
{
    return &_narmock_state_global_output_message.public;
}
