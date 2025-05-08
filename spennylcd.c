#include <linux/atomic.h>
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/kernel.h>
#include <linux/module.h> /* Needed by all modules */
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/mod_devicetable.h>
#include <asm/errno.h>

#include "spennylcd.h"

static atomic_t is_open = ATOMIC_INIT(0);

static const struct i2c_device_id spenny_dev_ids[] = {
    { "spennylcd", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, spenny_dev_ids);

static const struct of_device_id spenny_of_table[] = {
    { .compatible = "spenny,spennylcd" },
    { }
};
MODULE_DEVICE_TABLE(of, spenny_of_table);

struct spenny_lcd_data {
    struct i2c_client* client;
};

static struct spenny_lcd_data spenny_lcd_data = { };

static ssize_t
spenny_read_dev(struct file *f, char __user *buf, size_t user_bufsiz, loff_t *offset)
{
    // This device is write-only
    return -EINVAL;
}

static ssize_t
spenny_write_dev(struct file *f, const char __user *buf, size_t user_bufsiz, loff_t *offset)
{
    int max_bufsiz = 10;
    char our_buf[11];
    int bufsiz = user_bufsiz;
    if (bufsiz > max_bufsiz)
        bufsiz = max_bufsiz;

    if (copy_from_user(our_buf, buf, bufsiz))
        return -EFAULT;

    our_buf[bufsiz] = '\0';
    for (int i = 0; i < bufsiz && our_buf[i]; i++) {
	   if (our_buf[i] >= 0x20) {
		   int err = spenny_i2c_send_char(spenny_lcd_data.client, our_buf[i]);
		   if (err)
               return err;
	   }
    }

    *offset += bufsiz;
    return bufsiz;
}

static int
spenny_open_dev(struct inode *in, struct file *f)
{
    if (atomic_cmpxchg(&is_open, 0, 1)) {
        return -EBUSY;
    }
    try_module_get(THIS_MODULE);

    return 0;
}

static int
spenny_release_dev(struct inode *in, struct file *f)
{
    atomic_set(&is_open, 0);
    module_put(THIS_MODULE);
    return 0;
}

static struct file_operations fops = {
    .read = spenny_read_dev,
    .write = spenny_write_dev,
    .open = spenny_open_dev,
    .release = spenny_release_dev
};

static struct miscdevice spenny_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = SPENNY_DRV_NAME,
    .fops = &fops
};

static int
spenny_create_lcd_misc(struct i2c_client *client)
{
    int result = misc_register(&spenny_miscdev);
    if (result < 0) {
        pr_alert("failed to register device");
        return result;
    }

    spenny_lcd_data.client = client;
    return result;
}

static int
spenny_lcd_probe(struct i2c_client *client)
{
    int status = spenny_i2c_check_for_device(client);
    if (status < 0)
        return status;

    int init_status = spenny_i2c_init(client);
    if (init_status < 0)
        return init_status;

    pr_info("spennylcd found a device\n");
    return spenny_create_lcd_misc(client);
}

static void
spenny_lcd_remove(struct i2c_client *client)
{
    misc_deregister(&spenny_miscdev);
}

static struct i2c_driver spenny_i2c_drv = {
    .driver = {
        .name = SPENNY_DRV_NAME,
        .of_match_table = spenny_of_table
    },
    .probe = spenny_lcd_probe,
    .remove = spenny_lcd_remove,
    .id_table = spenny_dev_ids,
};

int __init spenny_dev_init(void)
{
    int i2c_res = i2c_add_driver(&spenny_i2c_drv);

    if (i2c_res) {
        pr_alert("failed to make i2c driver\n");
        return i2c_res;
    }

    return 0;
}
module_init(spenny_dev_init);

void __exit spenny_dev_exit(void)
{
    i2c_del_driver(&spenny_i2c_drv);
	misc_deregister(&spenny_miscdev);
}
module_exit(spenny_dev_exit);

MODULE_AUTHOR("Spencer Leslie <spencerdleslie@gmail.com>");
MODULE_DESCRIPTION("Simple i2c driver for an lcd screen");
MODULE_LICENSE("GPL");
