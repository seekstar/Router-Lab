ip netns add pc1
ip netns add r1
ip netns add r2
ip netns add r3
ip netns add pc2

ip link add pc1r1 type veth peer name r1pc1
ip link add r1r2 type veth peer name r2r1
ip link add r2r3 type veth peer name r3r2
ip link add r3pc2 type veth peer name pc2r3

ip link set pc1r1 netns pc1
ip link set r1pc1 netns r1
ip link set r1r2 netns r1
ip link set r2r1 netns r2
ip link set r2r3 netns r2
ip link set r3r2 netns r3
ip link set r3pc2 netns r3
ip link set pc2r3 netns pc2

ip netns exec pc1 ip link set pc1r1 up
ip netns exec r1 ip link set r1pc1 up
ip netns exec r1 ip link set r1r2 up
ip netns exec r2 ip link set r2r1 up
ip netns exec r2 ip link set r2r3 up
ip netns exec r3 ip link set r3r2 up
ip netns exec r3 ip link set r3pc2 up
ip netns exec pc2 ip link set pc2r3 up

ip netns exec pc1 ip addr add 192.168.1.2/24 dev pc1r1
ip netns exec pc2 ip addr add 192.168.5.1/24 dev pc2r3

ip netns exec pc1 ip route add default via 192.168.1.1
ip netns exec pc2 ip route add default via 192.168.5.2

# deal with: pcap_inject failed with send: Message too long
# ip netns exec pc1 ethtool -K pc1r1 tso off
# ip netns exec r1 ethtool -K r1pc1 tso off
# ip netns exec r1 ethtool -K r1r2 tso off
# ip netns exec r2 ethtool -K r2r1 tso off
# ip netns exec r2 ethtool -K r2r3 tso off
# ip netns exec r3 ethtool -K r3r2 tso off
# ip netns exec r3 ethtool -K r3pc2 tso off
# ip netns exec pc2 ethtool -K pc2r3 tso off

# deal with: TCP checksum incorrect
ip netns exec pc1 ethtool -K pc1r1 tx off
ip netns exec r1 ethtool -K r1pc1 tx off
ip netns exec r1 ethtool -K r1r2 tx off
ip netns exec r2 ethtool -K r2r1 tx off
ip netns exec r2 ethtool -K r2r3 tx off
ip netns exec r3 ethtool -K r3r2 tx off
ip netns exec r3 ethtool -K r3pc2 tx off
ip netns exec pc2 ethtool -K pc2r3 tx off

# ip netns exec pc1 ping 192.168.1.1
# ip netns exec pc1 ping 192.168.5.1
