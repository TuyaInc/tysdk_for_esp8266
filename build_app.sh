#!/bin/sh
# 传入的参数说明：
#
# $1 - APP产物路径，如： demos/product
# $2 - APP产物名字，如： product
# $3 - APP产物版本，如： x.x.x
# $4 - APP产物包路径，如： output/dist/product1_1.0.0
# $5 - APP类型，如： debug  release 默认 release

#
# 例：
# ./build_app.sh demos/product product x.x.x   output/dist/product_x.x.x debug

APP_PATH=$1
APP_NAME=$2
APP_VERSION=$3
echo APP_PATH=$APP_PATH
echo APP_NAME=$APP_NAME
echo APP_VERSION=$APP_VERSION


fatal() {
    echo -e "\033[0;31merror: $1\033[0m"
    exit 1
}


[ -z $APP_PATH ] && fatal "no app path!"
[ -z $APP_NAME ] && fatal "no app name!"
[ -z $APP_VERSION ] && fatal "no version!"


DEBUG_FLAG=`echo $APP_VERSION | sed -n 's,^[0-9]\+\.\([0-9]\+\)\.[0-9]\+.*$,\1,p'`
BETA_FLAG=`echo $APP_VERSION | sed -n 's,^[0-9]\+\.[0-9]\+\.[0-9]\+\(.*$\),\1,p'`
echo "DEBUG_FLAG=$DEBUG_FLAG BETA_FLAG=$BETA_FLAG"
DEBUG=0
SET_FLAG=0

cd `dirname $0`


ROOT_DIR=$(pwd)
 
if [ -z "$1" ];then
        echo "please input app path!!!"
        exit 1
fi
if [ -z "$2" ];then
        echo "please input the app name!!!"
        exit 1
fi
if [ -z "$3" ];then
        echo "please input the app sversion!!!"
        exit 1
fi



if [ -z "$4" ];then
        # 通过环境变量传递生成的产物包全路径名称
		export CI_PACKAGE_PATH="$(pwd)/output/dist/$2_$3/"
		echo "$CI_PACKAGE_PATH!!!"	
else
        # 通过环境变量传递生成的产物包全路径名称
	if [ $4 = "debug" ];then
                DEBUG=1
		SET_FLAG=1
		export CI_PACKAGE_PATH="$(pwd)/output/dist/$2_$3/"
		echo "$CI_PACKAGE_PATH!!!"
	else
		if [ $4 = "release" ];then
			DEBUG=0
			SET_FLAG=1
			export CI_PACKAGE_PATH="$(pwd)/output/dist/$2_$3/"
			echo "$CI_PACKAGE_PATH!!!"
		else       
			export CI_PACKAGE_PATH="$(pwd)/$4"
			echo "$CI_PACKAGE_PATH!!!"
		fi
	fi		
fi
if [ $SET_FLAG = 0 ];then              
	if [ -z "$5" ];then	
		if [ $((DEBUG_FLAG%2)) = 0 ]; then
			DEBUG=1
		else
			DEBUG=0	
		fi	
	else
	        if [ $5 = "debug" ];then
	                DEBUG=1
	        else
	                if [ $5 = "release" ];then
				DEBUG=0
			fi
	        fi
	fi
fi 
APP_REAL_VERSION=`echo $APP_VERSION | sed -n 's,\(^[0-9]\+\.[0-9]\+\.[0-9]\+\).*$,\1,p'`
echo "APP_REAL_VERSION=$APP_REAL_VERSION"
export SDK_TARGET_APP_PATH="$1"
echo "SDK_TARGET_APP_PATH=$SDK_TARGET_APP_PATH"
export TARGET_PLATFORM=xtensa-lx106-elf
echo "TARGET_PLATFORM=$TARGET_PLATFORM"
CUR_DIR=$(cd $(dirname $0); pwd)   
export PATH=$PATH:$CUR_DIR/platforms/$TARGET_PLATFORM/bin
echo "PATH=$PATH"
#兼容老的产品Makfile
if [ -d platforms/${TARGET_PLATFORM}/build/build_sh_compatible ]; then
	rm -fr   ./platforms/build_sh
	mkdir -p ./platforms/build_sh
	cp -fr   ./platforms/${TARGET_PLATFORM}/build/build_sh_compatible/*  ./platforms/build_sh
fi
echo -e "******************Build APP Begin********************"
cd ./platforms/$TARGET_PLATFORM/build/build_sh
if [ $DEBUG = 0 ];then
        ./build_app_release_1m.sh $2 $APP_REAL_VERSION
else
        ./build_app_1m.sh $2 $APP_REAL_VERSION
fi                
cd $CUR_DIR
echo -e "$(cd $(dirname $0); pwd)"
echo -e "******************Build Finish***********************"



