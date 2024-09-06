echo "########################## START #####################"

type xrun
if [ $? -eq 0 ]; then
echo "########### FOUND XCELIUM ###############"
XCELIUM=1
fi

type qrun
if [ $? -eq 0 ]; then
echo "########### FOUND QUESTA ###############"
QUESTA=1
fi

type riviera
if [ $? -eq 0 ]; then
echo "########### FOUND RIVIERA ###############"
RIVIERA=1
fi

type vcs
if [ $? -eq 0 ]; then
echo "########### FOUND VCS ###############"
VCS=1
# M32=-m32
fi

type gcc
gcc $M32 -c -fpic hello_vpi.c -o hello_vpi.o
gcc $M32 -shared -o libvpi.so hello_vpi.o
realpath libvpi.so

if [ $XCELIUM ]; then
CADENCE_ENABLE_AVSREQ_44905_PHASE_1=1 xrun -64 -xmlibdirname dut.d dut.v -access +rwc -loadvpi /home/runner/libvpi.so:my_func
fi

if [ $QUESTA ]; then
vlog dut.v
vsim -batch -voptargs=+acc=npr -pli /home/runner/libvpi.so work.dut -do "run -all"
fi

if [ $RIVIERA ]; then
vlib work
vlog dut.v -pli /home/runner/libvpi.so
vsim +access +w_nets -interceptcoutput -c -pli /home/runner/libvpi.so -do "run -all" dut
fi

if [ $VCS ]; then
vcs -licqueue '-sverilog' -debug_access+all +vpi -load /home/runner/libvpi.so:my_func -full64 design.sv
./simv +vcs+lic+wait
fi