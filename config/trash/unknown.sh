sudo iptables-save | sudo tee /etc/iptables.sav
#     Edit /etc/rc.local and add the following lines before the "exit 0" line: 
#	iptables-restore < /etc/iptables.sav

sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"

