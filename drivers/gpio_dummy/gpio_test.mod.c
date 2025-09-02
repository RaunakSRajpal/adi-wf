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
	{ 0x92997ed8, "_printk" },
	{ 0x5f754e5a, "memset" },
	{ 0xe97c4103, "ioremap" },
	{ 0xc86ef25b, "proc_create" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xedc03953, "iounmap" },
	{ 0x377a2a0c, "proc_remove" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x1f6e58fe, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "577817DFC5AC3615635A5B5");
