#include <asm/errno.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include "spennylcd.h"

static int
spenny__i2c_send_byte_and_latch(struct i2c_client* client, u8 byte)
{
    int status = 0;

    // TODO: do I need to delay?
    u8 b = byte;
    status = i2c_smbus_write_byte(client, b);
    if (status < 0)
        return status;

    udelay(1);
    b = byte | SPENNY_E_SET;
    status = i2c_smbus_write_byte(client, b);
    if (status < 0)
        return status;

    udelay(1);
    b = byte;
    status = i2c_smbus_write_byte(client, b);

    udelay(1);

    return status;
}

int
spenny_i2c_send_char(struct i2c_client* client, char c)
{
    u8 upper = SPENNY_CMD_BYTE_DEFAULT | SPENNY_RS_SET | ((c >> 4) << 4);
    u8 lower = SPENNY_CMD_BYTE_DEFAULT | SPENNY_RS_SET | ((c & 0x0F) << 4);

    int status;
    status = spenny__i2c_send_byte_and_latch(client, upper);
    if (status < 0)
        return status;

    status = spenny__i2c_send_byte_and_latch(client, lower);

    return status;
}

int
spenny_i2c_init(struct i2c_client* client)
{
    // TODO: this is ugly as all get out

    // select 4-bit mode
    // start by sending 8-bit mode twice, then switch to select 4-bit mode
    // as per the datasheet
    spenny__i2c_send_byte_and_latch(client, 0x38);
    msleep(5);
    spenny__i2c_send_byte_and_latch(client, 0x38);
    udelay(120);
    spenny__i2c_send_byte_and_latch(client, 0x38);
    spenny__i2c_send_byte_and_latch(client, 0x28);
    // should now have 4 bit mode

    // fn set: 4 bits, 2-line, 5x8 chars
    spenny__i2c_send_byte_and_latch(client, 0x28);
    spenny__i2c_send_byte_and_latch(client, 0x88);

    // display off
    spenny__i2c_send_byte_and_latch(client, 0x08);
    spenny__i2c_send_byte_and_latch(client, 0x88);

    // display clear
    spenny__i2c_send_byte_and_latch(client, 0x08);
    spenny__i2c_send_byte_and_latch(client, 0x18);

    // entry mode set
    spenny__i2c_send_byte_and_latch(client, 0x08);
    spenny__i2c_send_byte_and_latch(client, 0x68);

    // home
    spenny__i2c_send_byte_and_latch(client, 0x08);
    spenny__i2c_send_byte_and_latch(client, 0x28);

    // display on
    spenny__i2c_send_byte_and_latch(client, 0x08);
    spenny__i2c_send_byte_and_latch(client, 0xE8);

    return 0;
}

int
spenny_i2c_check_for_device(struct i2c_client* client)
{
    // make sure it's an I2C device
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
        return -EIO;

    // and that we can read from it
    s32 status = i2c_smbus_read_byte(client);
    if (status < 0)
        return status;

    return 0;
}
