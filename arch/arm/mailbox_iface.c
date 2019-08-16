#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>

#include "mailbox.h"

int  __attribute__((aligned(16))) mbox[128];
static int mb_index = 0;
void PropertyInit(){
    mbox[MB_SIZE] = 12;
    mbox[MB_REQUEST_OR_RESPONSE] = 0;
    mb_index = 2;
    mbox[mb_index] = 0;
}

void *memcpy(void *restrict s1, const void *restrict s2, size_t n) {
  char *c1 = (char *)s1;
  const char *c2 = (const char *)s2;
  for (size_t i = 0; i < n; ++i)
    c1[i] = c2[i];
  return s1;
}

void PropertyAddTag(mailbox_tag_t tag, ...)
{
    va_list args;
    va_start(args, tag);

    mbox[mb_index++] = tag;

    switch (tag)
    {
        case TAG_GET_VOLTAGE:
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
        default:
            mb_index--;
    }
    va_end(args);

}
uint32_t PropertyProcess(void)
{
    uint32_t result;
    mbox[MB_SIZE] = (mb_index + 1) << 2;
    mbox[MB_REQUEST_OR_RESPONSE] = 0;
    Mailbox0Write(MB0_TAGS_ARM_TO_VC, mbox);
    return result;
}

mailbox_property_t* PropertyGet(mailbox_tag_t tag)
{
    static mailbox_property_t property;
    int* tag_buffer = NULL;

    property.tag = tag;

    int index = 2;
    while (index < (mbox[MB_SIZE] >> 2))
    {
        if (mbox[index] == tag )
        {
            tag_buffer = &mbox[index];
            break;
        }
        index += (mbox[index + 1] >> 2) + 3;
    }
    if (tag_buffer == NULL)
        return NULL;
    
    property.byte_length = tag_buffer[T_ORESPONSE] & 0xffff;
    
    memcpy(property.data.buffer_8, &tag_buffer[T_OVALUE], property.byte_length);

    return &property;
}
