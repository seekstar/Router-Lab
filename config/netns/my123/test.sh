ip netns exec pc1 ping 192.168.5.1

ip netns exec pc2 ping 192.168.1.2

ip netns exec pc1 nc -l 80
ip netns exec pc2 nc 192.168.1.2 80

ip netns exec pc2 nc -l 80
ip netns exec pc1 nc 192.168.5.1 80

ip netns exec pc2 iperf3 -s
ip netns exec pc1 iperf3 -c 192.168.5.1
ip netns exec pc1 iperf3 -c 192.168.5.1 -u -l 16 -t 5 -b 1G

