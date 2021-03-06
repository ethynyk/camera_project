/*
 * Flash on CPE mx29lv
 *
 * $Id: cpe-flash.c,v 1.1.1.1 2006/03/13 10:29:36 jedy Exp $
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/arch/io.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>


#define NB_OF(x)  (sizeof(x)/sizeof(x[0]))

#define FLASH_START			0x80400000
#define FLASH_SIZE			(4*1024*1024)
#define FLASH_WIDTH			4


__u8 mx29lv_read8(struct map_info *map, unsigned long ofs)
{
	return __raw_readb(map->map_priv_1 + ofs);
}

__u16 mx29lv_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 mx29lv_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void mx29lv_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
//	udelay(10);
}

void mx29lv_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
//	udelay(10);
}

void mx29lv_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
//	udelay(10);
}

void mx29lv_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	// printk ("mx29lv_copy_from: 0x%x@0x%x -> 0x%x\n", len, from, to);
	//memcpy_fromio(to, map->map_priv_1 + from, len);
	memcpy(to, (void *)(map->map_priv_1 + from), len);
//	udelay(10);
}

void mx29lv_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	while(len) {
		__raw_writeb(*(unsigned char *) from, map->map_priv_1 + to);
		from++;
		to++;
		len--;
	}
//	udelay(10);
}


static struct mtd_info *flash_mtd;

struct map_info mx29lv_flash_map = {
	name: "flash",
	size: FLASH_SIZE,
	buswidth: FLASH_WIDTH,
	read8: mx29lv_read8,
	read16: mx29lv_read16,
	read32: mx29lv_read32,
	copy_from: mx29lv_copy_from,
	write8: mx29lv_write8,
	write16: mx29lv_write16,
	write32: mx29lv_write32,
	copy_to: mx29lv_copy_to
};

struct resource mx29lv_flash_resource = {
	name:   "Flash",
	start:  FLASH_START,
	end:    FLASH_START + FLASH_SIZE - 1,
	flags:  IORESOURCE_IO | IORESOURCE_BUSY,
};

static struct mtd_partition mx29lv_flash_partition[] = {
	{ name: "boot",		offset: 0x0, 		size: 0x40000 },
	{ name: "kernel",   offset: 0x40000, 	size: 0xc0000 },					// 0x80440000 ~
	{ name: "romfs", 	offset: 0x100000, 	size: 0x200000 },					// 0x80500000 ~
	{ name: "jffs2",    offset: 0x300000, 	size: 0x100000 }					// 0x80600000 ~ 0x80800000
};

static int __init init_mx29lv_flash (void)
{
	int err;
	
	if (request_resource (&ioport_resource, &mx29lv_flash_resource)) 
	{
		printk(KERN_NOTICE "Failed to reserve mx29lv FLASH space\n");
		err = -EBUSY;
		goto out;
	}
	
	mx29lv_flash_map.map_priv_1 = (unsigned long)ioremap(FLASH_START, FLASH_SIZE);
	if (!mx29lv_flash_map.map_priv_1) 
	{
		printk(KERN_NOTICE "Failed to ioremap mx29lv FLASH space\n");
		err = -EIO;
		goto out_resource;
	}

	flash_mtd = do_map_probe("jedec_probe", &mx29lv_flash_map);
#ifdef not_complete_yet
	if (!flash_mtd) {
		flash_mtd = do_map_probe("map_rom", &mx29lv_flash_map);
		if (flash_mtd)
			flash_mtd->erasesize = 0x10000;
	}
#endif /* end_of_not */
	if (!flash_mtd) {
		printk("FLASH probe failed\n");
		err = -ENXIO;
		goto out_ioremap;
	}

	flash_mtd->module = THIS_MODULE;

#ifdef not_complete_yet	
	if (add_mtd_device(flash_mtd)) {
		printk("FLASH device addition failed\n");
		err = -ENOMEM;
		goto out_probe;
	}
#endif /* end_of_not */
	if (add_mtd_partitions(flash_mtd, mx29lv_flash_partition, NB_OF(mx29lv_flash_partition)))
	{
		printk("FLASH device addition failed\n");
		err = -ENOMEM;
		goto out_probe;
	}

	return 0;

out_probe:
	map_destroy(flash_mtd);
	flash_mtd = 0;
out_ioremap:
	iounmap((void *)mx29lv_flash_map.map_priv_1);
out_resource:
	release_resource (&mx29lv_flash_resource);
out:
	return err;
}




#ifdef not_complete_yet
static struct mtd_info *sram_mtd;

struct map_info mx29lv_sram_map = {
	name: "SRAM",
	size: SRAM_SIZE,
	buswidth: SRAM_WIDTH,
	read8: mx29lv_read8,
	read16: mx29lv_read16,
	read32: mx29lv_read32,
	copy_from: mx29lv_copy_from,
	write8: mx29lv_write8,
	write16: mx29lv_write16,
	write32: mx29lv_write32,
	copy_to: mx29lv_copy_to
};

struct resource mx29lv_sram_resource = {
	name:   "SRAM",
	start:  SRAM_START,
	end:    SRAM_START + SRAM_SIZE - 1,
	flags:  IORESOURCE_IO | IORESOURCE_BUSY,
};

static int __init init_mx29lv_sram (void)
{
	int err;
	
	if (request_resource (&ioport_resource, &mx29lv_sram_resource)) {
		printk(KERN_NOTICE "Failed to reserve mx29lv SRAM space\n");
		err = -EBUSY;
		goto out;
	}
	
	mx29lv_sram_map.map_priv_1 = (unsigned long)ioremap(SRAM_START, SRAM_SIZE);
	if (!mx29lv_sram_map.map_priv_1) {
		printk(KERN_NOTICE "Failed to ioremap mx29lv SRAM space\n");
		err = -EIO;
		goto out_resource;
	}

	sram_mtd = do_map_probe("map_ram", &mx29lv_sram_map);
	if (!sram_mtd) {
		printk("SRAM probe failed\n");
		err = -ENXIO;
		goto out_ioremap;
	}

	sram_mtd->module = THIS_MODULE;
	sram_mtd->erasesize = 16;
	
	if (add_mtd_device(sram_mtd)) {
		printk("SRAM device addition failed\n");
		err = -ENOMEM;
		goto out_probe;
	}
		
	return 0;

out_probe:
	map_destroy(sram_mtd);
	sram_mtd = 0;
out_ioremap:
	iounmap((void *)mx29lv_sram_map.map_priv_1);
out_resource:
	release_resource (&mx29lv_sram_resource);
out:
	return err;
}

#endif /* end_of_not */




#ifdef not_complete_yet
static struct mtd_info *bootrom_mtd;

struct map_info mx29lv_bootrom_map = {
	name: "BootROM",
	size: BOOTROM_SIZE,
	buswidth: BOOTROM_WIDTH,
	read8: mx29lv_read8,
	read16: mx29lv_read16,
	read32: mx29lv_read32,
	copy_from: mx29lv_copy_from,
};

struct resource mx29lv_bootrom_resource = {
	name:   "BootROM",
	start:  BOOTROM_START,
	end:    BOOTROM_START + BOOTROM_SIZE - 1,
	flags:  IORESOURCE_IO | IORESOURCE_BUSY,
};


static int __init init_mx29lv_bootrom (void)
{
	int err;
	
	if (request_resource (&ioport_resource, &mx29lv_bootrom_resource)) {
		printk(KERN_NOTICE "Failed to reserve mx29lv BOOTROM space\n");
		err = -EBUSY;
		goto out;
	}
	
	mx29lv_bootrom_map.map_priv_1 = (unsigned long)ioremap(BOOTROM_START, BOOTROM_SIZE);
	if (!mx29lv_bootrom_map.map_priv_1) {
		printk(KERN_NOTICE "Failed to ioremap mx29lv BootROM space\n");
		err = -EIO;
		goto out_resource;
	}

	bootrom_mtd = do_map_probe("map_rom", &mx29lv_bootrom_map);
	if (!bootrom_mtd) {
		printk("BootROM probe failed\n");
		err = -ENXIO;
		goto out_ioremap;
	}

	bootrom_mtd->module = THIS_MODULE;
	bootrom_mtd->erasesize = 0x10000;
	
	if (add_mtd_device(bootrom_mtd)) {
		printk("BootROM device addition failed\n");
		err = -ENOMEM;
		goto out_probe;
	}
		
	return 0;

out_probe:
	map_destroy(bootrom_mtd);
	bootrom_mtd = 0;
out_ioremap:
	iounmap((void *)mx29lv_bootrom_map.map_priv_1);
out_resource:
	release_resource (&mx29lv_bootrom_resource);
out:
	return err;
}
#endif /* end_of_not */


void flash_dummy()
{
	int i;
	
	printk("enter flash_dummy\n");
}

static int __init init_mx29lv_maps(void)
{
#ifdef not_complete_yet
	printk(KERN_INFO "CPE mx29lv MTD mappings:\n  Flash 0x%x at 0x%x\n  SRAM 0x%x at 0x%x\n  BootROM 0x%x at 0x%x\n", 
	       FLASH_SIZE, FLASH_START, SRAM_SIZE, SRAM_START, BOOTROM_SIZE, BOOTROM_START);
#endif /* end_of_not */


	init_mx29lv_flash();
	
#ifdef not_complete_yet
	init_mx29lv_sram();
	init_mx29lv_bootrom();
#endif /* end_of_not */

	return 0;
}
	

static void __exit cleanup_mx29lv_maps(void)
{
#ifdef not_complete_yet
	if (sram_mtd) {
		del_mtd_device(sram_mtd);
		map_destroy(sram_mtd);
		iounmap((void *)mx29lv_sram_map.map_priv_1);
		release_resource (&mx29lv_sram_resource);
	}
#endif /* end_of_not */

	if (flash_mtd) {
		//del_mtd_device(flash_mtd);
		del_mtd_partitions(flash_mtd);
		map_destroy(flash_mtd);
		iounmap((void *)mx29lv_flash_map.map_priv_1);
		release_resource (&mx29lv_flash_resource);
	}

#ifdef not_complete_yet
	if (bootrom_mtd) {
		del_mtd_device(bootrom_mtd);
		map_destroy(bootrom_mtd);
		iounmap((void *)mx29lv_bootrom_map.map_priv_1);
		release_resource (&mx29lv_bootrom_resource);
	}
#endif /* end_of_not */
}

module_init(init_mx29lv_maps);
module_exit(cleanup_mx29lv_maps);

MODULE_AUTHOR("lmc83");
MODULE_DESCRIPTION("ARM mx29lv map driver");
MODULE_LICENSE("GPL");
