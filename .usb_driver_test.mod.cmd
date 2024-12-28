savedcmd_/home/ubuntu/ldd/usb_driver_test.mod := printf '%s\n'   usb_driver_test.o | awk '!x[$$0]++ { print("/home/ubuntu/ldd/"$$0) }' > /home/ubuntu/ldd/usb_driver_test.mod
