#include <time.h>
#include <unistd.h>

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

void fake_output_message(char *message)
{
    printf("fake %s\n", message);
}

TEST(output_message_function)
{
    CAPTURE_OUTPUT(original_output) { output_message("hello"); }
    ASSERT_EQ(original_output, "hello\n");

    MOCK(output_message)->mock_return();

    CAPTURE_OUTPUT(mocked_output) { output_message("hello"); }
    ASSERT_EQ(mocked_output, "");

    MOCK(output_message)->mock_implementation(fake_output_message);

    CAPTURE_OUTPUT(fake_output) { output_message("hello"); }
    ASSERT_EQ(fake_output, "fake hello\n");
}

TEST(time_function)
{
    time_t start = time(NULL);

    MOCK(time)->mock_return(0);
    ASSERT_EQ(time(NULL), 0);

    MOCK(time)->disable_mock();
    ASSERT_GE(time(NULL), start);
}

TEST(pipe_function)
{
    MOCK(pipe)->mock_return(42);

    int fd[2] = { 0, 0 };

    ASSERT_EQ(pipe(fd), 42);
    ASSERT_EQ(fd[0], 0);
    ASSERT_EQ(fd[1], 0);

    MOCK(pipe)->disable_mock();

    ASSERT_EQ(pipe(fd), 0);
    ASSERT_NE(fd[0], 0);
    ASSERT_NE(fd[1], 0);

    close(fd[0]);
    close(fd[1]);
}

TEST_GROUP(mocking_tests, { add_function, output_message_function, time_function, pipe_function });
