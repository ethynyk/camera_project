
/*
 * linux/include/asm-arm/arch-pl1097/memory.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 */

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#include <linux/config.h>

/*
 * Task size: 3GB
 */
#define TASK_SIZE       (0xc0000000UL)
#define TASK_SIZE_26    (0x04000000UL)

/*
 * This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE (TASK_SIZE / 3)

/*
 * Page offset: 3GB
 */
#define PAGE_OFFSET	(0xc0000000)

/*
 * Physical DRAM offset is 0x0000_0000
 */
#define PHYS_OFFSET	(0x00000000)

#ifndef __ASSEMBLY__
extern unsigned long DRAM_SIZE;
#endif

/*
 * We take advantage of the fact that physical and virtual address can be the
 * same.  The NUMA code is handling the large holes that might exist between
 * all memory banks.
 */
#define __virt_to_phys__is_a_macro
#define __phys_to_virt__is_a_macro
#define __virt_to_phys(x)	((unsigned long)(x) - PAGE_OFFSET + PHYS_OFFSET)
#define __phys_to_virt(x)	((unsigned long)(x) + PAGE_OFFSET - PHYS_OFFSET)


/*
 * Virtual view <-> DMA view memory address translations
 * virt_to_bus: Used to translate the virtual address to an
 *		address suitable to be passed to set_dma_addr
 * bus_to_virt: Used to convert an address for DMA operations
 *		to an address that the kernel can use.
 *
 * On the SA1100, bus addresses are equivalent to physical addresses.
 */
#define __virt_to_bus__is_a_macro
#define __bus_to_virt__is_a_macro
#define __virt_to_bus(x)	 __virt_to_phys(x)
#define __bus_to_virt(x)	 __phys_to_virt(x)


#endif
