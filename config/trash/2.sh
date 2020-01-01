sudo /etc/init.d/networking stop
sudo ip addr add 192.168.0.100/24 dev eno1
sudo ip route add default via 192.168.0.1
sudo cp /etc/resolv.conf /etc/resolv.conf.backup
#sudo nano /etc/dhcp3/dhclient.conf


