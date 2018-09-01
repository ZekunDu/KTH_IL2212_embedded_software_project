# File: run_hello_mpsoc.sh

# This script
#   - creates a board support package for the hardware platform
#   - compiles the application and generates an executable 
#   - downloads the hardware to the board
#   - starts a terminal window
#   - downloads the software and starts the application
# 
# Start the script with sh ./run_hello_mpsoc.sh

#!/bin/bash

echo "Setting up the build environment..."

APP=hello_mpsoc
BSP_DIR=../../bsp
BSP=mpsoc_hello_mpsoc
CORE_DIR=../../hardware/de2_nios2_mpsoc
SOPCINFO=nios2_mpsoc
SOF=de2_nios2_mpsoc
CPU=cpu_
NODES=5


if [[ `md5sum $CORE_DIR/$SOPCINFO.*` == `cat $CORE_DIR/.update.md5` ]] && [[ `md5sum $(basename $0)` == `cat .run.md5` ]]; then 
	echo "SOPCINFO file was not modified during this session. Will not rebuild the bsp files."
	REMAKE_BSP=false
else
	echo "SOPCINFO file was modified. Will remake the BSP files."
	REMAKE_BSP=true
	md5sum $CORE_DIR/$SOPCINFO.* > $CORE_DIR/.update.md5
	md5sum $(basename $0) > .run.md5
fi


echo "" >> log.txt
echo "[Compiling code for ${CPU}0]" > log.txt
echo "" >> log.txt


# Create BSP-Package for all processors

if [ ! -d $BSP_DIR/${BSP}_0 ] || [ "$REMAKE_BSP" = true ]; then
	nios2-bsp hal $BSP_DIR/${BSP}_0 $CORE_DIR/$SOPCINFO.sopcinfo \
		  --cpu-name ${CPU}0 \
		  --set hal.make.bsp_cflags_debug -g \
		  --set hal.make.bsp_cflags_optimization -Os \
		  --set hal.enable_small_c_library 1 \
		  --set hal.enable_reduced_device_drivers 1 \
		  --set hal.enable_lightweight_device_driver_api 1 \
		  --set hal.enable_sopc_sysid_check 1 \
		  --set hal.max_file_descriptors 4 \
		  --default_sections_mapping sram
	echo " "
	echo "BSP package creation finished"
	echo " "
fi

cd $BSP_DIR/${BSP}_0

make 3>&1 1>>log.txt 2>&1

cd ../../app/$APP

# Create Application
nios2-app-generate-makefile --bsp-dir $BSP_DIR/${BSP}_0 --elf-name ${APP}_0.elf --src-dir src_0/ --set APP_CFLAGS_OPTIMIZATION -Os

# Create ELF-file
make 3>&1 1>>log.txt 2>&1

for (( i = 1; i < $NODES; i++ ))  do
echo "" >> log.txt
echo "[Compiling code for ${CPU}$i]" >> log.txt
echo "" >> log.txt

if [ ! -d $BSP_DIR/${BSP}_$i ] || [ "$REMAKE_BSP" = true ]; then
	nios2-bsp hal $BSP_DIR/${BSP}_$i $CORE_DIR/$SOPCINFO.sopcinfo \
		  --cpu-name ${CPU}$i \
		  --set hal.make.bsp_cflags_debug -g \
		  --set hal.make.bsp_cflags_optimization -Os \
		  --set hal.enable_small_c_library 1 \
		  --set hal.enable_reduced_device_drivers 1 \
		  --set hal.enable_lightweight_device_driver_api 1 \
		  --set hal.enable_sopc_sysid_check 1 \
		  --set hal.max_file_descriptors 4 \
		  --default_sections_mapping onchip_$i \
		  --set hal.sys_clk_timer none \
		  --set hal.timestamp_timer none \
		  --set hal.enable_exit false \
		  --set hal.enable_c_plus_plus false \
		  --set hal.enable_clean_exit false \
		  --set hal.enable_sim_optimize false
	echo " "
	echo "BSP package creation finished"
	echo " "
fi

# Create Application
nios2-app-generate-makefile --bsp-dir $BSP_DIR/${BSP}_$i --elf-name ${APP}_$i.elf --src-dir src_$i/ --set APP_CFLAGS_OPTIMIZATION -Os

# Create ELF-file
make 3>&1 1>>log.txt 2>&1

done

# Download Hardware to Board

echo ""
echo "***********************************************"
echo "Download hardware to board"
echo "***********************************************"
echo ""

nios2-configure-sof $CORE_DIR/$SOF.sof

# Start Nios II Terminal for each processor

for (( i = 0; i < $NODES; i++ ))  do

echo ""
echo "Start NiosII terminal ..."

xterm -title "$CPU$i" -e "nios2-terminal -i $i" &

done

for (( i = 0; i < $NODES; i++ ))  do

echo ""
echo "***********************************************"
echo "Download software to board"
echo "***********************************************"
echo ""

nios2-download -g ${APP}_$i.elf --cpu_name $CPU$i --jdi $CORE_DIR/$SOF.jdi

echo ""
echo "Statistics"
nios2-elf-size ${APP}_$i.elf

done

echo ""
echo "Code compilation errors are logged in 'log.txt'"
