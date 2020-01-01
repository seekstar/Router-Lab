# basepath=$(cd `dirname $0`; pwd)
cd ../../../Homework/boilerplate
gnome-terminal -x ip netns exec r1 ./boilerplate1
gnome-terminal -x ip netns exec r2 ./boilerplate2
gnome-terminal -x ip netns exec r3 ./boilerplate3
