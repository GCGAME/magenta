#include "libc.h"
#include <errno.h>
#include <magenta/syscalls.h>
#include <magenta/types.h>
#include <sys/mman.h>

static void dummy(void) {}
weak_alias(dummy, __vm_wait);

int __mprotect(void* addr, size_t len, int prot) {
    __vm_wait();

    uintptr_t ptr = (uintptr_t)addr;
    uint32_t mx_prot = 0;
    mx_prot |= (prot & PROT_READ) ? MX_VM_FLAG_PERM_READ : 0;
    mx_prot |= (prot & PROT_WRITE) ? MX_VM_FLAG_PERM_WRITE : 0;
    mx_prot |= (prot & PROT_EXEC) ? MX_VM_FLAG_PERM_EXECUTE : 0;
    mx_status_t status = _mx_vmar_protect(_mx_vmar_root_self(), ptr, len, mx_prot);
    if (!status)
        return 0;

    switch (status) {
    case ERR_ACCESS_DENIED:
        errno = EACCES;
        break;
    case ERR_INVALID_ARGS:
        errno = ENOTSUP;
        break;
    default:
        errno = EINVAL;
        break;
    }
    return -1;
}

weak_alias(__mprotect, mprotect);
