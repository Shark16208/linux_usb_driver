savedcmd_/home/ubuntu/ldd/usb_driver_final.mod := printf '%s\n'   usb_driver_final.o | awk '!x[$$0]++ { print("/home/ubuntu/ldd/"$$0) }' > /home/ubuntu/ldd/usb_driver_final.mod
