/*
NARMOCK_DECLARATIONS_BEGIN
*/

/* TODO: includes and guards */

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
    _narmock_state_type_add *(*mock_return_once)(int return_value);
    _narmock_state_type_add *(*mock_implementation_once)(int (*implementation)(int arg1, int arg2));
    _narmock_state_type_add *(*disable_mock)(void);
};

_narmock_state_type_add *_narwhal_mock_add();

/*
NARMOCK_DECLARATIONS_END
*/

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
_narmock_state_type_add *_narmock_function_mock_return_once_add(int return_value);
_narmock_state_type_add *_narmock_function_mock_implementation_once_add(int (*implementation)(int arg1, int arg2));
_narmock_state_type_add *_narmock_function_disable_mock_add();

_narmock_state_private_type_add _narmock_state_global_add =
{
    .public = {
        .mock_return = _narmock_function_mock_return_add,
        .mock_implementation = _narmock_function_mock_implementation_add,
        .mock_return_once = _narmock_function_mock_return_once_add,
        .mock_implementation_once = _narmock_function_mock_implementation_once_add,
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

_narmock_state_type_add *_narmock_function_mock_return_once_add(int return_value)
{
    (void)return_value;

    return &_narmock_state_global_add.public;
}

_narmock_state_type_add *_narmock_function_mock_implementation_once_add(int (*implementation)(int arg1, int arg2))
{
    (void)implementation;

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
