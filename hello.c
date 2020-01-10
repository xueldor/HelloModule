#include <linux/init.h>
#include <linux/module.h>

static char* book_name="dissecting Linux Device Driver";
module_param(book_name,charp,S_IRUGO);//charp:字符指针

static int book_num = 4000;
module_param(book_num,int,S_IRUGO);

static int __init hello_init(void)
{
	printk(KERN_INFO"Hello World enter\n");//内核空间输出用printk,用户空间用printf
	printk(KERN_INFO"book name:%s\n",book_name);
	printk(KERN_INFO"book num:%d\n",book_num);
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
