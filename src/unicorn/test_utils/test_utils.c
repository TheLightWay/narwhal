#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "unicorn/test/test.h"
#include "unicorn/test_utils/test_utils.h"
#include "unicorn/utils.h"


UnicornOutputCapture _unicorn_default_output_capture =
{
    .initialization_phase = true,
    .stdout_backup = -1,
    .stderr_backup = -1,
    .pipe = { -1, -1 },
    .parent = NULL
};


/*
 * Initialize capture
 */

static void initialize_output_capture(UnicornOutputCapture *capture)
{
    if (pipe(capture->pipe) == -1)
    {
        fprintf(stderr, "Failed to create capture pipe.\n");
        exit(EXIT_FAILURE);
    }

    capture->parent = _unicorn_current_test->output_capture;
    _unicorn_current_test->output_capture = capture;

    capture->stdout_backup = dup(STDOUT_FILENO);
    capture->stderr_backup = dup(STDERR_FILENO);

    while (dup2(capture->pipe[1], STDOUT_FILENO) == -1 && errno == EINTR);
    while (dup2(capture->pipe[1], STDERR_FILENO) == -1 && errno == EINTR);
}


/*
 * Finalize capture
 */

static void finalize_output_capture(UnicornOutputCapture *capture, char **output_buffer)
{
    bool terminator_written = write(capture->pipe[1], "", 1) == 1;

    close(capture->pipe[1]);

    dup2(capture->stdout_backup, STDOUT_FILENO);
    dup2(capture->stderr_backup, STDERR_FILENO);

    if (terminator_written)
    {
        FILE *stream = fdopen(capture->pipe[0], "r");

        ssize_t output_length = unicorn_util_read_stream(stream, output_buffer) - 1;

        if (write(STDOUT_FILENO, *output_buffer, output_length) != output_length) {
            fprintf(stderr, "Failed to write captured output to stdout");
        }

        fclose(stream);
    }
    else
    {
        fprintf(stderr, "Failed to write to capture pipe.\n");
    }

    _unicorn_current_test->output_capture = capture->parent;

    close(capture->pipe[0]);
}


/*
 * Main capture function
 */

bool unicorn_capture_output(UnicornOutputCapture *capture, char **output_buffer)
{
    fflush(stdout);
    fflush(stderr);

    if (capture->initialization_phase)
    {
        initialize_output_capture(capture);
        capture->initialization_phase = false;

        return true;
    }
    else
    {
        finalize_output_capture(capture, output_buffer);
        auto_free(*output_buffer);

        return false;
    }
}
