#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2b22e0c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xcae3c359, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0xbec3bd0f, __VMLINUX_SYMBOL_STR(nf_register_hook) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x76cd9e64, __VMLINUX_SYMBOL_STR(dev_queue_xmit) },
	{ 0xe113bbbc, __VMLINUX_SYMBOL_STR(csum_partial) },
	{ 0xd36649d9, __VMLINUX_SYMBOL_STR(dev_get_by_name) },
	{ 0xd7a1ceb0, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0xc77bace2, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x1c3db152, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0x1b6314fd, __VMLINUX_SYMBOL_STR(in_aton) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "30D916D4E925A18F4CC809E");
