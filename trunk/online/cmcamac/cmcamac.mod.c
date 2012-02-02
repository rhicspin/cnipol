#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x1a197155, "struct_module" },
	{ 0xfbeafc35, "usb_register_dev" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x5d03ae11, "usb_get_dev" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x19070091, "kmem_cache_alloc" },
	{ 0xab978df6, "malloc_sizes" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x470b36e4, "usb_find_interface" },
	{ 0xef440511, "usb_register_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0x3773d415, "usb_put_dev" },
	{ 0xf9a482f9, "msleep" },
	{ 0x2e60bace, "memcpy" },
	{ 0xc38ebbf2, "usb_bulk_msg" },
	{ 0x1b7d4074, "printk" },
	{ 0x8c667e2, "usb_deregister_dev" },
	{ 0x25da070, "snprintf" },
	{ 0xcff53400, "kref_put" },
	{ 0x5dfe8f1a, "unlock_kernel" },
	{ 0x5568be43, "lock_kernel" },
	{ 0x48ea01b4, "usb_deregister" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v1279p0064d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "57638A5384789DA4DF3ABF9");
