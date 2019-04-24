#!/bin/bash

echo "gen_misc.sh version 20150511"
echo ""

boot=new
echo "boot mode: $boot"
echo ""

app1=1
app2=2
echo ""

echo "STEP 1: choose spi speed(0=20MHz, 1=26.7MHz, 2=40MHz, 3=80MHz)"
echo "enter (0/1/2/3, default 2):"
read input

if [ -z "$input" ]; then
    spi_speed=40
elif [ $input == 0 ]; then
    spi_speed=20
elif [ $input == 1 ]; then
    spi_speed=26.7
elif [ $input == 3 ]; then
    spi_speed=80
else
    spi_speed=40
fi

echo "spi speed: $spi_speed MHz"
echo ""

echo "STEP 2: choose spi mode(0=QIO, 1=QOUT, 2=DIO, 3=DOUT)"
echo "enter (0/1/2/3, default 0):"
read input

if [ -z "$input" ]; then
    spi_mode=QIO
elif [ $input == 1 ]; then
    spi_mode=QOUT
elif [ $input == 2 ]; then
    spi_mode=DIO
elif [ $input == 3 ]; then
    spi_mode=DOUT
else
    spi_mode=QIO
fi

echo "spi mode: $spi_mode"
echo ""

echo "STEP 3: choose spi size and map"
echo "    0= 512KB( 256KB+ 256KB)"
echo "    2=1024KB( 512KB+ 512KB)"
echo "    3=2048KB( 512KB+ 512KB)"
echo "    4=4096KB( 512KB+ 512KB)"
echo "    5=2048KB(1024KB+1024KB)"
echo "    6=4096KB(1024KB+1024KB)"
echo "enter (0/2/3/4/5/6, default 2):"
read input

if [ -z "$input" ]; then
    spi_size_map=2
    echo "spi size: 1024KB"
    echo "spi ota map:  512KB + 512KB"
elif [ $input == 0 ]; then
    spi_size_map=0
    echo "spi size: 512KB"
    echo "spi ota map:  256KB + 256KB"
elif [ $input == 3 ]; then
    spi_size_map=3
    echo "spi size: 2048KB"
    echo "spi ota map:  512KB + 512KB"
elif [ $input == 4 ]; then
    spi_size_map=4
    echo "spi size: 4096KB"
    echo "spi ota map:  512KB + 512KB"
elif [ $input == 5 ]; then
    spi_size_map=5
    echo "spi size: 2048KB"
    echo "spi ota map:  1024KB + 1024KB"
elif [ $input == 6 ]; then
    spi_size_map=6
    echo "spi size: 4096KB"
    echo "spi ota map:  1024KB + 1024KB"
else
    spi_size_map=2
    echo "spi size: 1024KB"
    echo "spi ota map:  512KB + 512KB"
fi

echo "#!/bin/sh" > build_app.sh
echo "boot=$boot" >> build_app.sh
echo "app=$app" >> build_app.sh
echo "spi_speed=$spi_speed" >> build_app.sh
echo "spi_mode=$spi_mode" >> build_app.sh
echo "spi_size_map=$spi_size_map" >> build_app.sh

echo "if [ -z \"\$1\" ];then" >> build_app.sh
echo "        echo \"please input the app bin name(no suffix \\\".bin\\\")!!!\"" >> build_app.sh
echo "        exit 1" >> build_app.sh
echo "else" >> build_app.sh
echo "        APP_BIN_NAME=\$1" >> build_app.sh
echo "fi" >> build_app.sh

echo "if [ -z \"\$2\" ];then" >> build_app.sh
echo "        echo \"please input the app sw version(for example:the format is \"1.1.1\")!!!\"" >> build_app.sh
echo "        exit 1" >> build_app.sh
echo "else" >> build_app.sh
echo "        USER_SW_VER=\$2" >> build_app.sh
echo "fi" >> build_app.sh

echo "echo \"\"" >> build_app.sh
echo "echo \"start...\"" >> build_app.sh
echo "echo \"\"" >> build_app.sh
echo "set -e" >> build_app.sh
echo "make -C ./package clean && make -C ./package install INSALL=../../bin/upgrade USER_SW_VER=\$USER_SW_VER" >> build_app.sh
echo "make clean APP_BIN_NAME=\$APP_BIN_NAME;\\" >> build_app.sh
echo "make COMPILE=gcc BOOT=$boot APP=$app1 SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map APP_BIN_NAME=\$APP_BIN_NAME USER_SW_VER=\$USER_SW_VER" >> build_app.sh
echo "make clean APP_BIN_NAME=\$APP_BIN_NAME;\\" >> build_app.sh
echo "make COMPILE=gcc BOOT=$boot APP=$app2 SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map APP_BIN_NAME=\$APP_BIN_NAME USER_SW_VER=\$USER_SW_VER" >> build_app.sh
chmod +x build_app.sh
