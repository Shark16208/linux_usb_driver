#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

// Driver info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sai Vittal Jadav");
MODULE_DESCRIPTION("USB Keyboard Driver with Interrupt Transfers");

// Define device-specific VID and PID
#define USB_VENDOR_ID 0x1234
#define USB_PRODUCT_ID 0x5678

// Max packet size for interrupt endpoint
#define MAX_PACKET_SIZE 8

// Main model of usb keyboard
struct usb_keyboard
{
    struct usb_device *udev;
    unsigned char *irq_buffer;
    dma_addr_t irq_dma;
    struct urb *irq_urb;
    int irq_interval;
};

// Device ID table with terminating entry
static struct usb_device_id keyboard_table[] = {
    {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
    {}};
MODULE_DEVICE_TABLE(usb, keyboard_table);

// Interrupt callback
static void keyboard_irq(struct urb *urb)
{
    struct usb_keyboard *kbd = urb->context;
    if (urb->status == 0)
    { // Check if transfer was successful
        printk(KERN_INFO "Key press detected: %02x\n", kbd->irq_buffer[2]);
    }
    else
    {
        printk(KERN_WARNING "Interrupt transfer failed: %d\n", urb->status);
    }
    // Resubmit the URB to continue polling
    if (usb_submit_urb(urb, GFP_ATOMIC))
    {
        printk(KERN_ERR "Failed to resubmit interrupt URB\n");
    }
}

// Probe function when usb first detected
static int keyboard_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_keyboard *kbd;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int pipe, maxp;

    iface_desc = interface->cur_altsetting;
    endpoint = &iface_desc->endpoint[0].desc;

    kbd = kzalloc(sizeof(struct usb_keyboard), GFP_KERNEL);
    if (!kbd)
    {
        printk(KERN_ERR "Cannot allocate memory for usb_keyboard struct\n");
        return -ENOMEM;
    }

    kbd->udev = usb_get_dev(interface_to_usbdev(interface));
    kbd->irq_interval = endpoint->bInterval;
    kbd->irq_buffer = usb_alloc_coherent(kbd->udev, MAX_PACKET_SIZE, GFP_ATOMIC, &kbd->irq_dma);
    if (!kbd->irq_buffer)
    {
        printk(KERN_ERR "Cannot allocate irq_buffer\n");
        kfree(kbd);
        return -ENOMEM;
    }

    kbd->irq_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!kbd->irq_urb)
    {
        printk(KERN_ERR "Cannot allocate irq_urb\n");
        usb_free_coherent(kbd->udev, MAX_PACKET_SIZE, kbd->irq_buffer, kbd->irq_dma);
        kfree(kbd);
        return -ENOMEM;
    }

    pipe = usb_rcvintpipe(kbd->udev, endpoint->bEndpointAddress);
    maxp = usb_maxpacket(kbd->udev, pipe);

    usb_fill_int_urb(kbd->irq_urb, kbd->udev, pipe, kbd->irq_buffer, maxp > MAX_PACKET_SIZE ? MAX_PACKET_SIZE : maxp,
                     keyboard_irq, kbd, endpoint->bInterval);
    kbd->irq_urb->transfer_dma = kbd->irq_dma;
    kbd->irq_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    usb_set_intfdata(interface, kbd);

    if (usb_submit_urb(kbd->irq_urb, GFP_KERNEL))
    {
        printk(KERN_ERR "Cannot submit irq_urb\n");
        usb_free_urb(kbd->irq_urb);
        usb_free_coherent(kbd->udev, MAX_PACKET_SIZE, kbd->irq_buffer, kbd->irq_dma);
        kfree(kbd);
        return -EIO;
    }

    printk(KERN_INFO "USB Keyboard (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}

// Disconnect function when usb removed
static void keyboard_disconnect(struct usb_interface *interface)
{
    struct usb_keyboard *kbd;

    kbd = usb_get_intfdata(interface);
    usb_set_intfdata(interface, NULL);

    usb_kill_urb(kbd->irq_urb);
    usb_free_urb(kbd->irq_urb);
    usb_free_coherent(kbd->udev, MAX_PACKET_SIZE, kbd->irq_buffer, kbd->irq_urb->transfer_dma);
    usb_put_dev(kbd->udev);
    kfree(kbd);

    printk(KERN_INFO "USB Keyboard removed\n");
}
// USB driver structure
static struct usb_driver keyboard_driver = {
    .name = "usb_hid_keyboard",
    .id_table = keyboard_table,
    .probe = keyboard_probe,
    .disconnect = keyboard_disconnect,
};

// Module initialization function
static int __init keyboard_init(void)
{
    printk(KERN_INFO "Initializing USB HID Keyboard Driver\n");
    return usb_register(&keyboard_driver);
}

// Module cleanup function
static void __exit keyboard_exit(void)
{
    printk(KERN_INFO "Exiting USB HID Keyboard Driver\n");
    usb_deregister(&keyboard_driver);
}

module_init(keyboard_init);
module_exit(keyboard_exit);