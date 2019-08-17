#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "mailbox.h"
#include "stdio.h"

int __attribute__((aligned(16))) mbox[128];
static int mb_index = 0;
void PropertyInit() {
    mbox[MB_SIZE] = 12;               // 0: Emmboxy mailbox size
    mbox[MB_REQUEST_OR_RESPONSE] = 0; // 1: Request
    mb_index = 2;                     // Index set to 2
    mbox[mb_index] = 0;               // 2: Must be 0
}

void *memcpy(void *restrict s1, const void *restrict s2,
             size_t n) { // Copy memory
    char *c1 = (char *)s1;
    const char *c2 = (const char *)s2;
    for (size_t i = 0; i < n; ++i)
        c1[i] = c2[i];
    return s1;
}

void PropertyAddTag(mailbox_tag_t tag, ...) {
    printf("PropertyAddTag(): Adding tag 0x%08X\n", tag);
    va_list args;
    va_start(args, tag);

    mbox[mb_index++] = tag; // index++: tag

    switch (tag) {
    case TAG_GET_FIRMWARE_VERSION:
    case TAG_GET_BOARD_MODEL:
    case TAG_GET_BOARD_REVISION:
    case TAG_GET_BOARD_MAC_ADDRESS:
    case TAG_GET_BOARD_SERIAL:
    case TAG_GET_ARM_MEMORY:
    case TAG_GET_VC_MEMORY:
    case TAG_GET_DMA_CHANNELS:
        /* Provide an 8-byte buffer for the response */
        mbox[mb_index++] = 8;
        mbox[mb_index++] = 0; /* Request */
        mb_index += 2;
        break;

    case TAG_GET_CLOCKS:
    case TAG_GET_COMMAND_LINE:
        /* Provide a 256-byte buffer */
        mbox[mb_index++] = 256;
        mbox[mb_index++] = 0; /* Request */
        mb_index += 256 >> 2;
        break;

    case TAG_ALLOCATE_BUFFER:
        mbox[mb_index++] = 8;
        mbox[mb_index++] = 0; /* Request */
        mbox[mb_index++] = va_arg(args, int);
        mb_index += 1;
        break;

    case TAG_GET_PHYSICAL_SIZE:
    case TAG_SET_PHYSICAL_SIZE:
    case TAG_TEST_PHYSICAL_SIZE:
    case TAG_GET_VIRTUAL_SIZE:
    case TAG_SET_VIRTUAL_SIZE:
    case TAG_TEST_VIRTUAL_SIZE:
    case TAG_GET_VIRTUAL_OFFSET:
    case TAG_SET_VIRTUAL_OFFSET:
        mbox[mb_index++] = 8;
        mbox[mb_index++] = 0; /* Request */

        if ((tag == TAG_SET_PHYSICAL_SIZE) || (tag == TAG_SET_VIRTUAL_SIZE) ||
            (tag == TAG_SET_VIRTUAL_OFFSET) ||
            (tag == TAG_TEST_PHYSICAL_SIZE) || (tag == TAG_TEST_VIRTUAL_SIZE)) {
            mbox[mb_index++] = va_arg(args, int); /* Width */
            mbox[mb_index++] = va_arg(args, int); /* Height */
        } else {
            mb_index += 2;
        }
        break;

    case TAG_GET_ALPHA_MODE:
    case TAG_SET_ALPHA_MODE:
    case TAG_GET_DEPTH:
    case TAG_SET_DEPTH:
    case TAG_GET_PIXEL_ORDER:
    case TAG_SET_PIXEL_ORDER:
    case TAG_GET_PITCH:
        mbox[mb_index++] = 4;
        mbox[mb_index++] = 0; /* Request */

        if ((tag == TAG_SET_DEPTH) || (tag == TAG_SET_PIXEL_ORDER) ||
            (tag == TAG_SET_ALPHA_MODE)) {
            /* Colour Depth, bits-per-pixel \ Pixel Order State */
            mbox[mb_index++] = va_arg(args, int);
        } else {
            mb_index += 1;
        }
        break;

    case TAG_GET_OVERSCAN:
    case TAG_SET_OVERSCAN:
        mbox[mb_index++] = 16;
        mbox[mb_index++] = 0; /* Request */

        if ((tag == TAG_SET_OVERSCAN)) {
            mbox[mb_index++] = va_arg(args, int); /* Top pixels */
            mbox[mb_index++] = va_arg(args, int); /* Bottom pixels */
            mbox[mb_index++] = va_arg(args, int); /* Left pixels */
            mbox[mb_index++] = va_arg(args, int); /* Right pixels */
        } else {
            mb_index += 4;
        }
        break;

    default:
        mb_index--;
    }
    va_end(args);
}
uint32_t PropertyProcess(void) {
    printf("PropertyProcess()");
    uint32_t result;
    mbox[MB_SIZE] = (mb_index + 1) << 2; // MailBox size = index + 1 * 4
    mbox[MB_REQUEST_OR_RESPONSE] = 0;    // Request
    Mailbox0Write(MB0_TAGS_ARM_TO_VC, (uint32_t)mbox);
    result = Mailbox0Read(MB0_TAGS_ARM_TO_VC);
    printf(" completed with result %d\n", result);
    return result;
}

mailbox_property_t *PropertyGet(mailbox_tag_t tag) {
    static mailbox_property_t property;
    int *tag_buffer = NULL;

    property.tag = tag;

    int index = 2; // First tag
    while (index < (mbox[MB_SIZE] >> 2)) {
        if (mbox[index] == tag) {
            tag_buffer = &mbox[index]; // Address of mailbox tag
            break;
        }
        index += (mbox[index + 1] >> 2) + 3; // Next tag
    }
    if (tag_buffer == NULL) // No tags?
        return NULL;

    property.byte_length = tag_buffer[T_ORESPONSE] & 0xffff;

    memcpy(property.data.buffer_8, &tag_buffer[T_OVALUE], property.byte_length);

    return &property;
}
