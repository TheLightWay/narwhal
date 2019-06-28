#ifndef NARWHAL_MOCK_H
#define NARWHAL_MOCK_H

#define MOCK(function)              \
    ({                              \
        (void)function;             \
        _narwhal_mock_##function(); \
    })

#endif
