obj-m += mymodule.o

all: module dt
	echo Builded Device Tree Overlay and kernel module

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
dt: mymodule_overlay.dts
	dtc -@ -I dts -O dtb -o mymodule_overlay.dtbo mymodule_overlay.dts
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf mymodule_overlay.dtbo