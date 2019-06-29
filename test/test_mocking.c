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

TEST(output_message_function)
{
    CAPTURE_OUTPUT(original_output) { output_message("hello"); }
    ASSERT_EQ(original_output, "hello\n");

    MOCK(output_message)->mock_return();

    CAPTURE_OUTPUT(mocked_output) { output_message("hello"); }
    ASSERT_EQ(mocked_output, "");
}

TEST_GROUP(mocking_tests, { add_function, output_message_function });
