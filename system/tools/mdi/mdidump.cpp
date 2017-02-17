// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <magenta/mdi.h>

static void dump_node(int fd, int level);

static void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

static void dump_string(int fd) {
    printf("\"");
    while (1) {
        char ch;
        read(fd, &ch, sizeof(ch));
        if (ch) {
            printf("%c", ch);
         } else {
            break;
         }
     }
    printf("\"");
}

static void dump_array_node(int fd, int level, mdi_node_t& node) {
    uint32_t count = node.value.array.count;

    // offset of node start. element offsets are relative to this
    off_t node_start = lseek(fd, 0, SEEK_CUR) - sizeof(node);

    printf("[ ");
    switch (node.value.array.type) {
        case MDI_INT8:
            for (int i = 0; i < count; i++) {
                int8_t value;
                read(fd, &value, sizeof(value));
                printf("%d ", value);
            }
            break;
        case MDI_UINT8:
            for (int i = 0; i < count; i++) {
                uint8_t value;
                read(fd, &value, sizeof(value));
                printf("%u ", value);
            }
            break;
        case MDI_INT16:
            for (int i = 0; i < count; i++) {
                int16_t value;
                read(fd, &value, sizeof(value));
                printf("%d ", value);
            }
            break;
        case MDI_UINT16:
            for (int i = 0; i < count; i++) {
                uint16_t value;
                read(fd, &value, sizeof(value));
                printf("%u ", value);
            }
            break;
        case MDI_INT32:
            for (int i = 0; i < count; i++) {
                int32_t value;
                read(fd, &value, sizeof(value));
                printf("%d ", value);
            }
            break;
        case MDI_UINT32:
            for (int i = 0; i < count; i++) {
                uint32_t value;
                read(fd, &value, sizeof(value));
                printf("%u ", value);
            }
            break;
        case MDI_INT64:
            for (int i = 0; i < count; i++) {
                int64_t value;
                read(fd, &value, sizeof(value));
                printf("%" PRId64 " ", value);
            }
            break;
        case MDI_UINT64:
            for (int i = 0; i < count; i++) {
                uint64_t value;
                read(fd, &value, sizeof(value));
                printf("%" PRIu64 " ", value);
            }
            break;
        case MDI_BOOLEAN:
            for (int i = 0; i < count; i++) {
                int8_t value;
                read(fd, &value, sizeof(value));
                printf("%s ",  (value ? "true" : "false"));
            }
            break;
        case MDI_STRING:
            printf("\n");
            for (int i = 0; i < count; i++) {
                mdi_offset_t offset;
                // read element offset
                read(fd, &offset, sizeof(offset));
                off_t saved_offset = lseek(fd, 0, SEEK_CUR);
                lseek(fd, node_start + offset, SEEK_SET);
                print_indent(level + 1);
                dump_string(fd);
                printf("\n");
                lseek(fd, saved_offset, SEEK_SET);
            }
            print_indent(level);
            break;   
        case MDI_LIST:
        case MDI_ARRAY:
            printf("\n");
            for (int i = 0; i < count; i++) {
                mdi_offset_t offset;
                // read element offset
                read(fd, &offset, sizeof(offset));
                off_t saved_offset = lseek(fd, 0, SEEK_CUR);
                lseek(fd, node_start + offset, SEEK_SET);
                dump_node(fd, level + 1);
                lseek(fd, saved_offset, SEEK_SET);
            }
            print_indent(level);
            break;   
        default:
            fprintf(stderr, "bad array element type %d\n", node.value.array.type);
            abort();
    }

    printf("]");

    lseek(fd, node_start + node.length, SEEK_SET);
}

static void dump_node(int fd, int level) {
    mdi_node_t node;

    if (read(fd, &node, sizeof(node)) != sizeof(node)) {
        fprintf(stderr, "read failed!\n");
        abort();
    }

    mdi_type_t type = MDI_ID_TYPE(node.id);
    uint32_t id_num = MDI_ID_NUM(node.id);

    print_indent(level);

    switch (type) {
        case MDI_INT8:
            printf("int8(%u) = %d", id_num, node.value.i8);
            break;
        case MDI_UINT8:
            printf("uint8(%u) = %u", id_num, node.value.u8);
            break;
        case MDI_INT16:
            printf("int16(%u) = %d", id_num, node.value.i16);
            break;
        case MDI_UINT16:
            printf("uint16(%u) = %u", id_num, node.value.u16);
            break;
        case MDI_INT32:
            printf("int32(%u) = %d", id_num, node.value.i32);
            break;
        case MDI_UINT32:
            printf("uint32(%u) = %u", id_num, node.value.u32);
            break;
        case MDI_INT64:
            printf("int64(%u) = %" PRId64, id_num, node.value.i64);
            break;
        case MDI_UINT64:
            printf("uint64(%u) = %" PRIu64, id_num, node.value.u64);
            break;
        case MDI_BOOLEAN:
            printf("boolean(%u) = %s", id_num, (node.value.u8 ? "true" : "false"));
            break;
        case MDI_STRING: {
            off_t node_start = lseek(fd, 0, SEEK_CUR) - sizeof(node);
            printf("string(%u) = ", id_num);
            dump_string(fd);
            lseek(fd, node_start + node.length, SEEK_SET);
            break;
        }
        case MDI_LIST: {
            printf("list(%u) = {\n", id_num);
            uint32_t child_count = node.value.list_count;
            for (uint32_t i = 0; i < child_count; i++) {
                dump_node(fd, level + 1);
            }
            print_indent(level);
            printf("}");
            break;
        }
        case MDI_ARRAY:
            printf("array(%u) = ", id_num);
            dump_array_node(fd, level, node); 
            break;
        default:
            fprintf(stderr, "unknown type %d\n", type);
    }
    printf("\n");

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: mdidump <mdi-file-path>\n");
        return -1;
    }

    const char* path = argv[1];
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "error: unable to open %s\n", path);
        return -1;
    }

    dump_node(fd, 0);

    close(fd);

    return 0;
}