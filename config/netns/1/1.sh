ip netns add pc1
ip netns add r1
ip netns add pc2

ip link add pc1r1 type veth peer name r1pc1
ip link add r1pc2 type veth peer name pc2r1

ip link set pc1r1 netns pc1
ip link set r1pc1 netns r1
ip link set r1pc2 netns r3
ip link set pc2r1 netns pc2

ip netns exec pc1 ip link set pc1r1 up
ip netns exec r1 ip link set r1pc1 up
ip netns exec r1 ip link set r1pc2 up
ip netns exec pc2 ip link set pc2r1 up

ip netns exec pc1 ip addr add 192.168.1.2/24 dev pc1r1
ip netns exec r1 ip addr add 192.168.1.1/24 dev r1pc1
ip netns exec r1 ip addr add 192.168.5.2/24 dev r1pc2
ip netns exec pc2 ip addr add 192.168.5.1/24 dev pc2r1

ip netns exec pc1 ip route add default via 192.168.1.1
ip netns exec pc2 ip route add default via 192.168.5.2
