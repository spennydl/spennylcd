obj-m += spennylcd_mod.o
spennylcd_mod-objs = spennylcd.o spennylcd_i2c.o

PWD := $(CURDIR)

all:
	$(MAKE) -C ../linux-6.6.78/ M=$(PWD) modules

clean:
	$(MAKE) -C ../linux-6.6.78/ M=$(PWD) clean
