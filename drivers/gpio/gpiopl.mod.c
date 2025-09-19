#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xc4234f8b, "class_destroy" },
	{ 0xedc03953, "iounmap" },
	{ 0x92997ed8, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x415eabc9, "device_create" },
	{ 0xd395c9c0, "class_create" },
	{ 0xe97c4103, "ioremap" },
	{ 0x5f754e5a, "memset" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x194b2167, "__register_chrdev" },
	{ 0x4b03ed6, "device_destroy" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x1f6e58fe, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "54E574669CD470DA9BB848E");
