#include <stdint.h>

#define PAGE_TABLE_BASE_ADDR_MASK (0xffffc000)
#define VIRT_TO_PTE_INDEX(addr) ((addr)&0xfff00000 >> 18)
#define PTE_SECTION_NO_CACHE_AND_WB (0x0 << 2)
#define PTE_SECTION_DOMAIN_DEFAULT (0x0 << 5)
#define PTE_ALL_AP_SECTION_DEFAULT (0x1 << 10)
#define PTE_SECTION_DEFAULT (0x1 << 10)
#define PTE_SECTION_PHYSADDR_BASE_MASK (0xfff00000)
#define PTE_BITS_SECTION (0x2)
#define PTE_BASE_ADDR 0xE6F7FFC
#define PHYS_ADDR 0x508000
#define VIRT_ADDR 0xc0000000
#define MEM_MAP_SIZE 0x32000000

static uint32_t get_page_table_entry(uint32_t phys_addr) {
    return (phys_addr & PTE_SECTION_PHYSADDR_BASE_MASK) | PTE_BITS_SECTION;
}

static uint32_t get_page_table_entry_addr(uint32_t base_addr,
                                          uint32_t virt_addr) {
    return (base_addr & PAGE_TABLE_BASE_ADDR_MASK) |
           VIRT_TO_PTE_INDEX(virt_addr);
}
void init_mmu(void) {
    uint32_t pte;
    uint32_t pte_addr;
    int j;
    for (j = 0; j<MEM_MAP_SIZE>> 20; j++) {
        pte = get_page_table_entry(PHYS_ADDR + (j << 20));
        pte |= PTE_ALL_AP_SECTION_DEFAULT;
        pte |= PTE_SECTION_NO_CACHE_AND_WB;
        pte |= PTE_SECTION_DOMAIN_DEFAULT;
        pte_addr =
            get_page_table_entry_addr(PTE_BASE_ADDR, VIRT_ADDR + (j << 20));
        *(volatile uint32_t *)pte_addr = pte;
    }
}

void start_mmu(void) {
    unsigned int ttb = PTE_BASE_ADDR;
    asm("mcr p15,0,%0,c2,c0,0\n"
        "mvn r0,#0\n"
        "mcr p15,0,r0,c3,c0,0\n"
        "mov r0,#1\n"
        "mcr p15,0,r0,c1,c0,0\n"
        "mov r0,r0\n"
        "mov r0,r0\n"
        "mov r0,r0\n"
        :
        : "r"(ttb)
        : "r0");
}
