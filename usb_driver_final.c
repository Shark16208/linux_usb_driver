#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sai Vittal Jadav");
MODULE_DESCRIPTION("USB HID Keyboard Driver with Interrupt Transfers");

// Define device-specific VID and PID
#define USB_VENDOR_ID 0x1234
#define USB_PRODUCT_ID 0x5678

// Max packet size for interrupt endpoint
#define MAX_PACKET_SIZE 8

// Main model of usb keyboard
struct usb_keyboard
{
    struct usb_device *udev;
    struct urb *irq_urb;       
    unsigned char *irq_buffer; 
    int irq_interval;          
};

// Device ID table with terminating entry
static struct usb_device_id keyboard_table[] = {
    {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
    {} 
};
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
    struct usb_host_interface *host_iface;
    struct usb_endpoint_descriptor *endpoint;
    int pipe;

    printk(KERN_INFO "USB HID Keyboard connected\n");

    kbd = kzalloc(sizeof(struct usb_keyboard), GFP_KERNEL);
    if (!kbd)
    {
        printk(KERN_ERR "Failed to allocate memory for USB keyboard\n");
        return -ENOMEM;
    }

    kbd->udev = usb_get_dev(interface_to_usbdev(interface));
    host_iface = interface->cur_altsetting;

    // Find the interrupt IN endpoint
    endpoint = &host_iface->endpoint[0].desc;
    if (!usb_endpoint_is_int_in(endpoint))
    {
        printk(KERN_ERR "No interrupt IN endpoint found\n");
        kfree(kbd);
        return -ENODEV;
    }

    pipe = usb_rcvintpipe(kbd->udev, endpoint->bEndpointAddress);
    kbd->irq_interval = endpoint->bInterval;

    // Allocate interrupt buffer and URB
    kbd->irq_buffer = kzalloc(MAX_PACKET_SIZE, GFP_KERNEL);
    if (!kbd->irq_buffer)
    {
        printk(KERN_ERR "Failed to allocate IRQ buffer\n");
        kfree(kbd);
        return -ENOMEM;
    }

    kbd->irq_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!kbd->irq_urb)
    {
        printk(KERN_ERR "Failed to allocate URB\n");
        kfree(kbd->irq_buffer);
        kfree(kbd);
        return -ENOMEM;
    }

    // Initialize the interrupt URB
    usb_fill_int_urb(kbd->irq_urb, kbd->udev, pipe, kbd->irq_buffer,
                     MAX_PACKET_SIZE, keyboard_irq, kbd, kbd->irq_interval);

    // Submit the interrupt URB
    if (usb_submit_urb(kbd->irq_urb, GFP_KERNEL))
    {
        printk(KERN_ERR "Failed to submit interrupt URB\n");
        usb_free_urb(kbd->irq_urb);
        kfree(kbd->irq_buffer);
        kfree(kbd);
        return -EIO;
    }

    usb_set_intfdata(interface, kbd);
    return 0;
}

// Disconnect function when usb removed
static void keyboard_disconnect(struct usb_interface *interface)
{
    struct usb_keyboard *kbd = usb_get_intfdata(interface);

    usb_set_intfdata(interface, NULL);
    if (kbd)
    {
        usb_kill_urb(kbd->irq_urb); // Stop ongoing transfers
        usb_free_urb(kbd->irq_urb); // Free URB
        kfree(kbd->irq_buffer);     // Free buffer
        kfree(kbd);                 // Free structure
    }

    printk(KERN_INFO "USB HID Keyboard disconnected\n");
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