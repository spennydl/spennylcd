#ifndef SPENNYLCD_H
#define SPENNYLCD_H

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/i2c.h>

#define SPENNY_DRV_NAME "spennylcd"

#define SPENNY_LCD_IOEXP_REG 0

#define SPENNY_RS_SET (1)
#define SPENNY_RW_SET (1 << 1)
#define SPENNY_E_SET  (1 << 2)
#define SPENNY_BL_SET (1 << 3)
#define SPENNY_D4_SET (1 << 4)
#define SPENNY_D5_SET (1 << 5)
#define SPENNY_D6_SET (1 << 6)
#define SPENNY_D7_SET (1 << 7)

#define SPENNY_CMD_BYTE_DEFAULT SPENNY_BL_SET

int
spenny_i2c_init(struct i2c_client *client);
int
spenny_i2c_check_for_device(struct i2c_client *client);
int
spenny_i2c_send_char(struct i2c_client* client, char c);

#endif // SPENNYLCD_H
