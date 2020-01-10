# HelloModule
A simple kernel module program in linux

Linux设备驱动以内核模块的形式出现，因此先学会Linux内核模块变成。
测试系统: ubuntu16.04
任意目录作为代码工作空间，例如在～/hello目录下。
新建hello.c,代码清单：
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    //内核空间输出用printk,用户空间用printf
	printk(KERN_INFO"Hello World enter\n");
	return 0;
}

module_init(hello_init);

static void __exit hello_exit(void)
{
	printk(KERN_INFO"Hello World exit\n");
}
module_exit(hello_exit);

MODULE_AUTHOR("xue@163.com");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple Hello World Module!");
MODULE_ALIAS("A simple module!");


新建Makefile:
KVERS = $(shell uname -r)

#Kernel modules
obj-m += hello.o

build:kernel_modules

kernel_modules:
	$(info $(KVERS))
	$(info $(CURDIR))
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

clean: 
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean


makefile语法:
1）9、10、11、14行是shell命令(command)
2）build、kernel_modules、clean是生成目标(target)。command描述了target应该怎样生成。target既可以是文件，也可以是标签。
比如"hello.o:hello.c hello.h",表示hello.o文件依赖hello.c和hello.h两个文件
而13行clean是一个Label，通过make clean执行
3）第六行 "build:kernel_modules"表示build这个目标依赖于kernel_modules。因此在执行build时，如果kernel_modules中有一个文件比build新的话，就会先执行kernel_modules。
4）command以TAB符开头
5）输入make，执行的是第一个target
6）obj-m += hello.o
obj-y 为包含到 Linux 内核 vmlinux 中的目标文件列表；obj-m 为编译成模块的目标文件列表；obj-n 和 obj- 中的文件列表被忽略
7）$(info $(KVERS))，将$(KVERS)的值打印出来。用来调试makefile。
8）make -C XXXX M=XXX
当make的目标为all时，-C $(KDIR) 指明跳转到源码目录下读取那里的Makefile；M=$(PWD) 表明然后返回到当前目录继续读入、执行当前的Makefile。

问题:
root@xue-virtual-machine:~/W/c_workspace/HelloModule/src# make
4.15.0-72-generic
/home/xue/W/c_workspace/HelloModule/src
make -C /lib/modules/4.15.0-72-generic/build M=/home/xue/W/c_workspace/HelloModule/src modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-72-generic'
make[2]: *** No rule to make target '/home/xue/W/c_workspace/HelloModule/src/hello.c', needed by '/home/xue/W/c_workspace/HelloModule/src/hello.o'。 停止。
Makefile:1577: recipe for target '_module_/home/xue/W/c_workspace/HelloModule/src' failed
make[1]: *** [_module_/home/xue/W/c_workspace/HelloModule/src] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-72-generic'
Makefile:9: recipe for target 'kernel_modules' failed
make: *** [kernel_modules] Error 2

第六行报错信息，意思是hello.o依赖hello.c，hello.c不存在，并且makefile里没有定义生成hello.c的规则。
解决：一开始源代码文件是helloworld.c,改为hello.c即可。
makefile里没有定义hello.o依赖hello.c,该依赖是make自动推导出的。

加载：执行insmod ./hello.ko
卸载：rmmod hello
执行dmesg,可以看到日志输出：
[35014.377498] Hello World enter
[35456.212240] Hello World exit
[35460.373138] Hello World enter
[35518.251612] Hello World exit
dmesg会立即结束，想实时输出日志可以执行cat /proc/kmsg，然后另开一个终端，加载模块
补充:实测时发现cat /proc/kmsg有时漏日志，有时异常结束。建议还是用dmesg -c调试。-c：输出log然后清空dmesg。dmesg --help查看帮助

查看系统已经加载的模块:lsmod
root@xue-virtual-machine:~# lsmod
Module                  Size  Used by
hello                  16384  0
vmw_vsock_vmci_transport    32768  2
vsock                  36864  3 vmw_vsock_vmci_transport
binfmt_misc            20480  1


lsmod实际是读取并分析/proc/modules文件：
root@xue-virtual-machine:/proc# cat /proc/modules 
hello 16384 0 - Live 0xffffffffc0521000 (OE)
vmw_vsock_vmci_transport 32768 2 - Live 0xffffffffc050c000
vsock 36864 3 vmw_vsock_vmci_transport, Live 0xffffffffc0560000
binfmt_misc 20480 1 - Live 0xffffffffc0452000
snd_ens1371 28672 0 - Live 0xffffffffc0515000


已加载模块信息也存在与/sys/module目录：
root@xue-virtual-machine:/sys/module/hello# tree -a
.
├── coresize
├── holders
├── initsize
├── initstate
├── notes
│   └── .note.gnu.build-id
├── refcnt
├── sections
│   ├── .exit.text
│   ├── .gnu.linkonce.this_module
│   ├── .init.text
│   ├── __mcount_loc
│   ├── .note.gnu.build-id
│   ├── .rodata.str1.1
│   ├── .strtab
│   └── .symtab
├── srcversion
├── taint
└── uevent

3 directories, 16 files



modprobe命令：比insmod更强大的命令

模块之间的依赖关系位于/lib/modules/<kernel-version>/modules.dep文件中：
root@xue-virtual-machine:~# cat /lib/modules/4.15.0-72-generic/modules.dep
kernel/arch/x86/events/intel/intel-rapl-perf.ko:
kernel/arch/x86/events/intel/intel-cstate.ko:
kernel/arch/x86/kernel/cpu/mcheck/mce-inject.ko:
kernel/arch/x86/kernel/msr.ko:
kernel/arch/x86/kernel/cpuid.ko:
kernel/arch/x86/crypto/glue_helper.ko:
kernel/arch/x86/crypto/aes-x86_64.ko:
kernel/arch/x86/crypto/des3_ede-x86_64.ko: kernel/crypto/des_generic.ko
kernel/arch/x86/crypto/camellia-x86_64.ko: kernel/crypto/lrw.ko kernel/arch/x86/crypto/glue_helper.ko



查看一个ko文件的信息:modinfo <模块名>
root@xue-virtual-machine:~/W/c_workspace/HelloModule/src# modinfo hello.ko
filename:       /home/xue/W/c_workspace/HelloModule/src/hello.ko
alias:          A simple module!
description:    A simple Hello World Module!
license:        GPL v2
author:         xue@163.com
srcversion:     422EE176DB2FF61F7BE78A9
depends:        
retpoline:      Y
name:           hello
vermagic:       4.15.0-72-generic SMP mod_unload
