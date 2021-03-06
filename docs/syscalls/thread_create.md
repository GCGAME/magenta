# mx_thread_create

## NAME

thread_create - create a thread

## SYNOPSIS

```
#include <magenta/syscalls.h>

mx_status_t mx_thread_create(mx_handle_t process, const char* name,
                             uint32_t name_len, uint32_t flags, mx_handle_t* out);

```

## DESCRIPTION

**thread_create**() creates a thread within the specified process.

Upon success a handle for the new thread is returned.  The thread
will not start executing until *thread_start()* is called.

*name* is silently truncated to a maximum of *MX_MAX_NAME_LEN-1* characters.

When the last handle to a thread is closed, the thread is destroyed.

Thread handles may be waited on and will assert the signal
*MX_THREAD_SIGNALED* when the thread stops executing (due to
*thread_exit**() being called).

*process* is the controlling [process object](../objects/process.md) for the
new thread, which will become a child of that process.

## RETURN VALUE

On success, **thread_create**() returns NO_ERROR and a handle (via *out*) to
the new thread.  In the event of failure, a negative error value is returned.

## ERRORS

**ERR_INVALID_ARGS**  *name* was an invalid pointer, or *flags* was non-zero.

**ERR_NO_MEMORY**  (Temporary) Failure due to lack of memory.

## SEE ALSO

[handle_close](handle_close.md),
[handle_duplicate](handle_duplicate.md),
[object_wait_one](object_wait_one.md),
[object_wait_many](object_wait_many.md),
[thread_exit](thread_exit.md),
[thread_start](thread_start.md).
