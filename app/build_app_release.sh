#!/bin/sh
boot=new
app=
spi_speed=40
spi_mode=QIO
spi_size_map=2
enable8266tls=1
if [ -z "$1" ];then
        echo "please input the app bin name(no suffix \".bin\")!!!"
        exit 1
else
        APP_BIN_NAME=$1
fi
if [ -z "$2" ];then
        echo "please input the app sw version(for example:the format is "1.1.1")!!!"
        exit 1
else
        USER_SW_VER=$2
fi

echo ""
echo "start..."
echo ""

set -e
make -C ./package clean && make -C ./package install INSALL=../../bin/upgrade USER_SW_VER=$USER_SW_VER
make clean APP_BIN_NAME=$APP_BIN_NAME;\
make COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=2 APP_BIN_NAME=$APP_BIN_NAME USER_SW_VER=$USER_SW_VER DEBUG=0 ENABLE_8266TLS=$enable8266tls
make clean APP_BIN_NAME=$APP_BIN_NAME;\
make COMPILE=gcc BOOT=new APP=2 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=2 APP_BIN_NAME=$APP_BIN_NAME USER_SW_VER=$USER_SW_VER DEBUG=0 ENABLE_8266TLS=$enable8266tls

