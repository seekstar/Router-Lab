sudo ip addr add 192.168.0.123/24 dev eth0
sudo iptables -A FORWARD -o eno1 -i eth0 -s 192.168.0.0/24 -m conntrack --ctstate NEW -j ACCEPT
sudo iptables -A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
sudo iptables -t nat -F POSTROUTING
sudo iptables -t nat -A POSTROUTING -o eno1 -j MASQUERADE

