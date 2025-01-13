To short serial connect pin8 to pin10

With hexdump we can display the bytes written on our SD-Card:
sudo hexdump /dev/mmcblk0 | head

For creating a Device File, we will use the mknod command:
cd
sudo mknod mymmc b 179 0

See serial
screen /dev/ttyS0 9600

Every character you are typing is echoed back. Now, let's create a Device File for a Character Device:
cd
sudo mknod myserial c 4 64
sudo screen myserial 9600