obj-m += spennylcd_mod.o
spennylcd_mod-objs = spennylcd.o spennylcd_i2c.o

PWD := $(CURDIR)

KERNEL_DIR=../linux

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
