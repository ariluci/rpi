#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("A simple I2C Device driver with driver_data");

/* To write a driver for a device on linux you have to:
 * 			1.name all compatible devices
 *			2. impement probe and remove functions
 *			3. implement driver struct 
 *			4.regsiter your driver
 */

/* Step 1 - name all compatible devices (a, b in  this example)*/
struct my_data {
	char name[32];
	int i;
};

static struct my_data a = {
	"Device A",
	42,
}; 
static struct my_data b = {
	"Device B",
	123,
};
static struct i2c_device_id my_ids[] = {
	{"a-dev", (long unsigned int) &a},
	{"b-dev", (long unsigned int) &b},
	{},
};
MODULE_DEVICE_TABLE(i2c, my_ids);

/* Step 2 - Probe and rem function */
static int my_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct my_data *data = (struct my_data*)id->driver_data;
	printk("my_i2c_driver - %s data->i=%d\n", data->name, data->i);
	printk("my_i2c_driver - ID: 0x%x\n", i2c_smbus_read_byte_data(client, 0xd0));
	return 0;
}

static void my_remove(struct i2c_client *client)
{
	printk("my_i2c_driver - Removing device\n");
}

/* Step 3 Driver struct */
static struct i2c_driver my_driver= {
	.probe = my_probe,
	.remove = my_remove,
	.id_table = my_ids,
	.driver = {
		.name = "my-i2c-driver",
	}
};

/* Step 4 - Register driver*/
module_i2c_driver(my_driver);