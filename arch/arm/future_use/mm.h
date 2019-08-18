#include <stdint.h>
struct list_head {
    struct list_head *next, *prev;
};

struct page {
    uint32_t addr;
    uint32_t flags;
    int order;
    struct list_head list;
};
