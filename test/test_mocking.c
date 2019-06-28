#include "__mocks__.h"
#include "dummy_functions.h"
#include "narwhal/narwhal.h"

int fake_add(int x, int y)
{
    return x + y + 1;
}

TEST(add_function)
{
    MOCK(add)->mock_return(42);

    ASSERT_EQ(add(1, 2), 42);

    MOCK(add)->disable_mock();

    ASSERT_EQ(add(1, 2), 3);

    MOCK(add)->mock_implementation(fake_add);

    ASSERT_EQ(add(1, 2), 4);
}

TEST_GROUP(mocking_tests, { add_function });
