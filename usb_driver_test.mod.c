#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x18\x00\x00\x00\xd5\x5e\x8a\xfa"
	"usb_kill_urb\0\0\0\0"
	"\x18\x00\x00\x00\xbe\x8f\xa4\x8b"
	"usb_free_urb\0\0\0\0"
	"\x1c\x00\x00\x00\xea\x47\x95\x7f"
	"usb_free_coherent\0\0\0"
	"\x14\x00\x00\x00\x2c\x19\xe0\x90"
	"usb_put_dev\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x18\x00\x00\x00\xfa\x24\xd4\x82"
	"usb_deregister\0\0"
	"\x18\x00\x00\x00\xee\xf7\x98\x84"
	"usb_submit_urb\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x18\x00\x00\x00\x10\x03\x98\x24"
	"kmalloc_caches\0\0"
	"\x18\x00\x00\x00\xeb\x9d\x19\x1d"
	"kmalloc_trace\0\0\0"
	"\x14\x00\x00\x00\x47\x74\xd6\xf8"
	"usb_get_dev\0"
	"\x1c\x00\x00\x00\x98\x5c\x89\x40"
	"usb_alloc_coherent\0\0"
	"\x18\x00\x00\x00\x08\x6f\xa5\x5e"
	"usb_alloc_urb\0\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\x79\xa5\x12\xc5"
	"usb_register_driver\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x18\x00\x00\x00\xd7\xd3\x75\x6d"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v1234p5678d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "AD4B3002AAD4F871BED0F7E");
