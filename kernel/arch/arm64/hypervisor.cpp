// Copyright 2017 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <arch/hypervisor.h>
#include <magenta/errors.h>

mx_status_t arch_hypervisor_create(mxtl::unique_ptr<HypervisorContext>* context) {
    return ERR_NOT_SUPPORTED;
}
