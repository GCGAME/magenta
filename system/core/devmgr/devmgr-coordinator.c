// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "devcoordinator.h"
#include "devmgr.h"
#include "memfs-private.h"

#include <fs/vfs.h>

#include <ddk/device.h>
#include <ddk/driver.h>

#include <magenta/syscalls.h>
#include <magenta/types.h>
#include <magenta/processargs.h>

#include <mxio/debug.h>
#include <mxio/dispatcher.h>
#include <mxio/remoteio.h>
#include <mxio/vfs.h>

#include <magenta/listnode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// The devmgr coordinator is an rpc service which devhost processes
// use to inform the devmgr when devices are published or removed.
//
// This service makes these published devices visible via the
// device filesystem visible at /dev in the devmgr's root namespace.

// vnodes for root driver and protocols
static vnode_t* vnroot;
static vnode_t* vnclass;

static mxio_dispatcher_t* coordinator_dispatcher;

static mx_handle_t devhost_job_handle;

typedef struct device_ctx {
    mx_handle_t hdevice;
    uint32_t protocol_id;
    vnode_t* vnode;
    char name[MX_DEVICE_NAME_MAX];
} device_ctx_t;


#define PNMAX 16
static const char* proto_name(uint32_t id, char buf[PNMAX]) {
    switch (id) {
#define DDK_PROTOCOL_DEF(tag, val, name) case val: return name;
#include <ddk/protodefs.h>
    default:
        snprintf(buf, PNMAX, "proto-%08x", id);
        return buf;
    }
}

static const char* proto_names[] = {
#define DDK_PROTOCOL_DEF(tag, val, name) name,
#include <ddk/protodefs.h>
    NULL,
};

static void prepopulate_protocol_dirs(void) {
    const char** namep = proto_names;
    while (*namep) {
        vnode_t* vnp;
        memfs_create_device_at(vnclass, &vnp, *namep++, 0);
    }
}

static void do_publish(device_ctx_t* parent, device_ctx_t* ctx) {
    if (memfs_create_device_at(parent->vnode, &ctx->vnode, ctx->name, ctx->hdevice)) {
        printf("devmgr: could not add '%s' to devfs!\n", ctx->name);
        return;
    }

    char buf[PNMAX];
    const char* pname = proto_name(ctx->protocol_id, buf);

    // find or create a vnode for class/<pname>
    vnode_t* vnp;
    mx_status_t status;
    if ((status = memfs_create_device_at(vnclass, &vnp, pname, 0)) < 0) {
        printf("devmgr: could not link to '%s'\n", ctx->name);
    }

    const char* name = ctx->name;
    if ((ctx->protocol_id != MX_PROTOCOL_MISC) &&
        (ctx->protocol_id != MX_PROTOCOL_CONSOLE)) {
        // request a numeric name
        name = NULL;
    }

    if ((status = memfs_add_link(vnp, name, ctx->vnode)) < 0) {
        printf("devmgr: could not link to '%s'\n", ctx->name);
    }
}

static mx_status_t do_remote_create(const char* name, uint32_t protocol_id, device_ctx_t** out,
                                    mx_handle_t* _hdevice, mx_handle_t* _hrpc) {
    size_t len = strlen(name);
    if (len >= MX_DEVICE_NAME_MAX) {
        return ERR_INVALID_ARGS;
    }
    device_ctx_t* ctx;
    if ((ctx = calloc(1, sizeof(device_ctx_t))) == NULL) {
        return ERR_NO_MEMORY;
    }

    mx_handle_t hdevice[2];
    mx_handle_t hrpc[2];
    mx_status_t status;
    if ((status = mx_channel_create(0, &hdevice[0], &hdevice[1])) < 0) {
        free(ctx);
        return status;
    }
    if ((status = mx_channel_create(0, &hrpc[0], &hrpc[1])) < 0) {
        free(ctx);
        mx_handle_close(hdevice[0]);
        mx_handle_close(hdevice[1]);
        return status;
    }

    memcpy(ctx->name, name, len);
    ctx->name[len] = 0;
    ctx->protocol_id = protocol_id;
    ctx->hdevice = hdevice[1];

    if ((status = mxio_dispatcher_add(coordinator_dispatcher, hrpc[1], NULL, ctx)) < 0) {
        mx_handle_close(hdevice[0]);
        mx_handle_close(hdevice[1]);
        mx_handle_close(hrpc[0]);
        mx_handle_close(hrpc[1]);
        free(ctx);
        return status;
    }

    *_hdevice = hdevice[0];
    *_hrpc = hrpc[0];
    *out = ctx;
    return NO_ERROR;
}

static mx_status_t do_remote_add(device_ctx_t* parent, const char* name, uint32_t protocol_id,
                                 mx_handle_t hdevice, mx_handle_t hrpc) {

    size_t len = strlen(name);
    if (len >= MX_DEVICE_NAME_MAX) {
        return ERR_INVALID_ARGS;
    }
    device_ctx_t* ctx;
    if ((ctx = calloc(1, sizeof(device_ctx_t))) == NULL) {
        return ERR_NO_MEMORY;
    }
    memcpy(ctx->name, name, len);
    ctx->name[len] = 0;
    ctx->protocol_id = protocol_id;
    ctx->hdevice = hdevice;

    //printf("devmgr: new ctx %p(%s), parent: %p(%s)\n", ctx, ctx->name, parent, parent->name);
    mx_status_t status;
    if ((status = mxio_dispatcher_add(coordinator_dispatcher, hrpc, NULL, ctx)) < 0) {
        mx_handle_close(hdevice);
        mx_handle_close(hrpc);
        free(ctx);
        return status;
    }

    do_publish(parent, ctx);
    return NO_ERROR;
}

static mx_status_t do_remote_remove(device_ctx_t* dev, bool clean) {
    //printf("devmgr: del ctx %p(%s) %s\n", dev, dev->name, clean ? "" : "unexpected!");
    devfs_remove(dev->vnode);
    mx_handle_close(dev->hdevice);
    dev->vnode = NULL;
    dev->hdevice = 0;
    free(dev);
    return NO_ERROR;
}

// handle dev_coordinator_msgs from devhosts
static mx_status_t coordinator_handler(mx_handle_t h, void* cb, void* cookie) {
    device_ctx_t* dev = cookie;
    dev_coordinator_msg_t msg;
    mx_handle_t handles[2];
    mx_status_t status;

    if (h == 0) {
        do_remote_remove(dev, false);
        return NO_ERROR;
    }

    uint32_t dsz = sizeof(msg);
    uint32_t hcount = 2;
    if ((status = mx_channel_read(h, 0, &msg, dsz, &dsz, handles, hcount, &hcount)) < 0) {
        if (status == ERR_BAD_STATE) {
            return ERR_DISPATCHER_NO_WORK;
        }
        return status;
    }
    if (dsz != sizeof(msg)) {
        goto fail;
    }
    switch (msg.op) {
    case DC_OP_ADD:
        if (hcount != 2) {
            goto fail;
        }
        do_remote_add(dev, msg.name, msg.protocol_id, handles[0], handles[1]);
        return NO_ERROR;
    case DC_OP_REMOVE:
        if (hcount != 0) {
            goto fail;
        }
        do_remote_remove(dev, true);
        // positive return indicates clean shutdown
        return 1;
    case DC_OP_SHUTDOWN:
        devmgr_vfs_exit();
        mx_handle_close(h);
        return NO_ERROR;
    default:
        goto fail;
    }
    return NO_ERROR;
fail:
    printf("coordinator_handler: error %d\n", status);
    for (unsigned n = 0; n < hcount; n++) {
        mx_handle_close(handles[n]);
    }
    return ERR_IO;
}

void devmgr_init(mx_handle_t root_job) {
    printf("devmgr: init\n");

    vnroot = devfs_get_root();
    memfs_create_device_at(vnroot, &vnclass, "class", 0);
    prepopulate_protocol_dirs();

    mx_status_t status = mx_job_create(root_job, 0u, &devhost_job_handle);
    if (status < 0) {
        printf("unable to create devhost job\n");
    }

    mxio_dispatcher_create(&coordinator_dispatcher, coordinator_handler);
}

void devmgr_handle_messages(void) {
    device_ctx_t* root;
    mx_status_t status;
    mx_handle_t hdevice, hrpc;
    if ((status = do_remote_create("root", 0, &root, &hdevice, &hrpc)) < 0) {
        printf("devmgr: failed to create root rpc node\n");
        return;
    }
    root->vnode = vnroot;
    const char* args[2] = { "/boot/bin/devhost", "root" };
    devmgr_launch_devhost(devhost_job_handle, "devhost:root", 2, (char**)args, hdevice, hrpc);

    printf("devmgr: root ctx %p\n", root);
    mxio_dispatcher_run(coordinator_dispatcher);
}
