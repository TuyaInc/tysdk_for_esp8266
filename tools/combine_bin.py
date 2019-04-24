# ESP8266 ROM Bootloader Utility
# www.espressif.com
# iot.espressif.cn
# Copyright (C) 2014 Espressif Shanghai
#
# This program is a firmware download tool with a simple graphic user interface.
# Thanks to Fredrik Ahlberg's outstanding work of esptool.py
# Port form XTCOM,which is compiled via visual studio, to python so that it can also be running in linux
# 
# If any bug is figured out ,please send the bug description and console log to wangjialin@espressif.com or wangcostaud@hotmail.com


VERSION = 'V0.9+'  #used in fdownload_panel_method.py

mode_list=['%c'%0x0,'%c'%0x1,'%c'%0x2,'%c'%0x3]
speed_list=['%c'%0x0,'%c'%0x1,'%c'%0x2,'%c'%0xf]  

size_list=['%c'%0x0,'%c'%0x1,'%c'%0x2,'%c'%0x3,'%c'%0x4,'%c'%0x5,'%c'%0x6,'%c'%0x7]  
crystal_list=['%c'%0x0,'%c'%0x1,'%c'%0x2]

speed_list_d = [0x0,0x1,0x2,0xf]
size_list_d = [0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7]
crystal_list_d = [0x0,0x1,0x2]


"""combine the detached binary files into one"""
def combineBin(dl,target_name,mode,speed,size,crystal,disable_cfg):
    dl_list = dl
    print "dl_list: \n\r",dl_list
    print "target_name: \n\r",target_name
    print "mode: \n\r",mode
    print "speed: \n\r",speed
    print "size: \n\r",size
    print "crystal: \n\r",crystal
    print "disable_cfg: \n\r",disable_cfg

    offset_list = [addr_pair[1] for addr_pair in dl_list]
    offset_list.sort()
    print offset_list
    
    addr_list = [ ]
    for offset in offset_list:
        for addr_pair in dl_list:
            if offset == addr_pair[1]:
                addr_list.append(addr_pair[0])
                break
    print addr_list

    fw=file(target_name,'wb')
    current=0
    for i in range(len(offset_list)):
        print "i: ",i
        for j in range(current,offset_list[i] ):
            fw.write( "%c"%0xff)
        current=offset_list[i]
        fr=open(addr_list[i] , 'rb')
        data=fr.read()
        dlen=len(data)
        fr.close()
        if disable_cfg == 0:
            if offset_list[i]==0x0:
                if VERSION == 'V0.8':
                    data=( data[0:2]+mode_list[mode]+speed_list[speed]+data[4:] )
                elif VERSION == 'V0.9+':
                    size_speed = (size_list_d[size]<<4)|speed_list_d[speed]
                    print "size_speed :",size_speed
                    data=( data[0:2]+mode_list[mode]+'%c'%(size_speed)+data[4:] )
            if 'esp_init_data' in addr_list[i]:
                data = ( data[0:48]+crystal_list[crystal]+data[49:] )
        else:   
            print "========================"
            print "NOTE: BINARY CONFIG DISABLED, "
            print "JUST COMBINE THE ORIGINAL BINARIES"
            print "========================"
        fw.write(data)
        current += dlen
        
    fw.close()
        
            
