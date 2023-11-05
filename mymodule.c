#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/serdev.h>

MODULE_LICENSE("GPL");

static int probe(struct serdev_device *serdev);
static void remove(struct serdev_device *serdev);

//atributo "compatible" deve ter o mesmo valor do que está no arquivo de overlay
static struct of_device_id mymodule_ids[] = {
	{
		.compatible = "piuartdev"
	},
	//null entry no fim do array
	{}
};

MODULE_DEVICE_TABLE(of, mymodule_ids);

//declaração das operações do serdev, driver.name pode ser qualquer coisa, mas é melhor que seja algo relacionado ao modulo
static struct serdev_device_driver mymodule_driver = {
	.probe = probe,
	.remove = remove,
	.driver = {
		.name = "mymodule",
		.of_match_table = mymodule_ids
	}
};

//função recebe um char e printa ele no log do kernel
static int receive_buf(struct serdev_device *serdev, const unsigned char *buffer, size_t size) {
	printk("Received %ld bytes with \"%s\"\n", size, buffer);

	return size;
}

static const struct serdev_device_ops ops = {
	.receive_buf = receive_buf
};

//Essa função vai ser chamada quando o serdev for registrado (na função my_init)
static int probe(struct serdev_device *serdev) {
	int status;

	printk("now im in the probe function!\n");

	//registra a operação de receber dados
	serdev_device_set_client_ops(serdev, &ops);

	//tenta inicializar o serdev
	status = serdev_device_open(serdev);

	if(status > 1) {
		printk("error when opening the serial device!\n");
		return -1;
	}

	//determina o baud rate como 9600
	//um baud é uma medida de velocidade de sinalização e representa o número de mudanças na linha de transmissão ou eventos por segundo
	serdev_device_set_baudrate(serdev, 9600);

	//determina que não tem flow control
	serdev_device_set_flow_control(serdev, false);

	//determina que não tem bit de paridade
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	return 0;
}

//Essa função vai ser chamada quando o serdev for removido (na função my_exit)
static void remove(struct serdev_device *serdev) {
	printk("now im in the remove function!\n");

	//mata o serdev
	serdev_device_close(serdev);
}


static int __init my_init(void) {
	printk("Hello, Kernel!\n");

	if(serdev_device_driver_register(&mymodule_driver)) {
		printk("could not load driver!\n");
		return -1;
	}

	return 0;
}

static void __exit my_exit(void) {
	printk("Goodbye, Kernel\n");

	serdev_device_driver_unregister(&mymodule_driver);
}

module_init(my_init);
module_exit(my_exit);