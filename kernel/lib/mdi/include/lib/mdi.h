// Copyright 2017 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#pragma once

#include <stdbool.h>
#include <magenta/mdi.h>
#include <magenta/types.h>

typedef struct mdi_node_ref {
    mdi_node_t* node;
} mdi_node_ref_t;


// takes pointer to MDI header and returns reference to MDI root node
mx_status_t mdi_init(void* mdi_header, mdi_node_ref_t* out_ref);

#if EMBED_MDI
// returns reference to MDI root node from MDI embedded in kernel image
mx_status_t mdi_init_embedded(mdi_node_ref_t* out_ref);
#endif

// returns the type of a node
static inline mdi_type_t mdi_get_type(mdi_node_ref_t* ref) {
    return MDI_ID_TYPE(ref->node->id);
}

// node value accessors
mx_status_t mdi_node_get_int32(mdi_node_ref_t* ref, int32_t* out_value);
mx_status_t mdi_node_get_uint32(mdi_node_ref_t* ref, uint32_t* out_value);
mx_status_t mdi_node_get_uint64(mdi_node_ref_t* ref, uint64_t* out_value);
mx_status_t mdi_node_get_boolean(mdi_node_ref_t* ref, bool* out_value);
const char* mdi_node_get_string(mdi_node_ref_t* ref);

// array element accessors
mx_status_t mdi_array_get_int32(mdi_node_ref_t* ref, uint32_t index, int32_t* out_value);
mx_status_t mdi_array_get_uint32(mdi_node_ref_t* ref, uint32_t index, uint32_t* out_value);
mx_status_t mdi_array_get_uint64(mdi_node_ref_t* ref, uint32_t index, uint64_t* out_value);
mx_status_t mdi_array_get_boolean(mdi_node_ref_t* ref, uint32_t index, bool* out_value);
const char* mdi_array_get_string(mdi_node_ref_t* ref, uint32_t index);

uint32_t mdi_node_get_child_count(mdi_node_ref_t* ref);
