#!/usr/bin/python
import combine_bin
import sys


#	dl = sys.argv[1];
#[[u'/home/changcheng/tuya/compare/boot_v1.4(b1).bin', 0], [u'/home/changcheng/tuya/compare/sample_pk(1)_1.1.1.bin', 4096], [u'/home/changcheng/tuya/compare/esp_init_data_default.bin', 1032192], [u'/home/changcheng/tuya/compare/blank.bin', 1040384]]

#	target_name = sys.argv[2];
# target.bin

#	mode = sys.argv[3];
# QIO->0 QOUT->1 DIO->2 DOUT->3

#	speed = sys.argv[4];
#40->0 26.7->1 20->2 80->3

#	size = sys.argv[5];
#4->0 2->1 8->2 16->3 32->4 16C->5 32C->6

#	crystal = sys.argv[6];
#40->0 26->1 24->2

#	disable_cfg = sys.argv[7];
#disable_cfg->0

if __name__=='__main__':
	boot_bin = sys.argv[1];
	small_bin = sys.argv[2];
	init_bin = sys.argv[3];
	blank_bin = sys.argv[4];
	dl = [
		[boot_bin, 0],
		[small_bin, 4096],
		[init_bin, 0xfc000],
		[blank_bin, 0xfe000]	
	]
	
	target_name = sys.argv[5];
	mode = int(sys.argv[6]);
	speed = int(sys.argv[7]);
	size = int(sys.argv[8]);
	crystal = int(sys.argv[9]);
	disable_cfg = int(sys.argv[10]);

	combine_bin.combineBin(dl, target_name, mode,  speed, size, crystal, disable_cfg);
