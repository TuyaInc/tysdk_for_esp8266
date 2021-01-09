/***********************************************************
*  File: uf_flash_file.h 
*  Author: lql
*  Date: 20181213
***********************************************************/
#ifndef __UF_FLASH_FILE_H
    #define __UF_FLASH_FILE_H
    #include "stdlib.h"
    #include "string.h"
    #include "stdio.h"

#ifdef __cplusplus
        extern "C" {
#endif

#ifdef  __UF_FLASH_FILE_GLOBALS
    #define __UF_FLASH_FILE_EXT
#else
    #define __UF_FLASH_FILE_EXT extern
#endif



/***********************************************************
*************************micro define***********************
***********************************************************/
typedef unsigned char uf_char_t;
typedef signed char uf_int8_t;
typedef unsigned char uf_uint8_t;
typedef short uf_int16_t;
typedef unsigned short uf_uint16_t;
typedef signed int uf_int32_t;
typedef unsigned int uf_uint32_t;

typedef uf_int32_t uf_ret;
#define UF_OK 0
#define UF_INVALID_PARM 1
#define UF_MALLOC_ERR 2
#define UF_COM_ERR 3
#define UF_BLOCK_UNVALID 4
#define UF_CRC_ERR 5
#define UF_ENCRYPT_DECODE_ERR 6
#define UF_ENCRYPT_ENCODE_ERR 7
#define UF_R_NOT_FIND_FILE 8
#define UF_X_FILE_EXIST 9
#define UF_FILE_NUM_OUT_OF_RANGE 10
#define UF_FILE_SIZE_OUT_OF_RANGE 11
#define UF_FLASH_SIZE_OUT_OF_RANGE 12

#define UF_FILE_NOT_FOUND 13


typedef uf_uint8_t uf_area_t;
#define UF_DATA_AREA 0
#define UF_MANAGE_AREA 1
#define UF_SWAP_AREA 2

#define TY_FLASH_ENCRYPTION 0


typedef uf_uint8_t file_mode_t;
#define OPEN_W_MODE 0      //write only + creat if not exist + clear file when open
#define OPEN_W_ADD_MODE 1  //write + read + creat if not exist + clear file when open
#define OPEN_R_MODE 2      //read only + error if not exist
#define OPEN_R_ADD_MODE 3  //write + read + error if not exist
#define OPEN_A_MODE 4      //write only + creat if not exist + offset = file_size
#define OPEN_A_ADD_MODE 5  //write + read + creat if not exist + offset = file_size
#define OPEN_X_MODE 6      //write only + err if exist(open only not exist)
#define OPEN_X_ADD_MODE 7  //write + read + err if exist(open only not exist)
//b or t mode not support

#define AES_ENCRYPT_KEY_LEN 32
#define UF_FILE_NAME_LEN 31
#define UF_FILE_NAME_LMT (UF_FILE_NAME_LEN + 1)


typedef uf_ret (*flash_read_f)(uf_uint32_t addr, uf_uint8_t *dst, uf_uint32_t size);
typedef uf_ret (*flash_write_f)(uf_uint32_t addr, uf_uint8_t *src, uf_uint32_t size);
typedef uf_ret (*flash_erase_f)(uf_uint32_t addr, uf_uint32_t size);
typedef uf_uint32_t (*crc32_f)(void *data,uf_uint16_t len);

typedef struct {
    uf_uint32_t start_addr; // start address
    uf_uint32_t flash_sz; // flash size
}UF_BASE_INFO_S;


//#pragma pack (1)
#define UF_BF_MAGIC 0x98764321

#define FILE_SEC_GATHER_LMT 32
typedef struct {
    uf_int8_t uf_name[UF_FILE_NAME_LMT];
    uf_uint32_t file_size;
    uf_int16_t file_sec_gather[FILE_SEC_GATHER_LMT];
}UF_S;

typedef struct {
    uf_uint32_t magic;
    uf_uint32_t crc32;
    uf_uint32_t wr_count;
    uf_uint16_t sector_id;
    uf_uint16_t data_uf_cnt;
    UF_S data_uf[0];
}UF_MANG_SEC_S;
//#pragma pack ()


typedef struct {
    file_mode_t mode;
    uf_uint32_t uf_offset;
    UF_S uf_file; 
}UF_HAND_S;

#define UF_CON_DATA_SEC_LMT 3 // continuous flash data section
typedef struct {
    //uf_uint8_t encrypt_key[AES_ENCRYPT_KEY_LEN];
    uf_uint32_t sector_size;

    flash_read_f uf_read;
    flash_write_f uf_write;
    flash_erase_f uf_erase;
    crc32_f uf_crc32;

    uf_uint16_t  data_sects_arr_cnt;
    UF_BASE_INFO_S data_sects_arr[UF_CON_DATA_SEC_LMT];
}UF_FLASH_CFG_S;

typedef struct {
    uf_uint16_t start_idx;
    uf_uint16_t end_idx;
}UF_SEC_GATHER_S;

#define UF_MANAGE_AREA_SECTOR_NUM 1
#define UF_SWAP_AREA_SECTOR_NUM 1
#define UF_DATA_AREA_MIN_SECTOR_NUM 2

typedef struct {
    UF_FLASH_CFG_S uff_cfg;

    uf_uint16_t data_sec_total;
    uf_uint16_t data_sec_free;
    uf_uint16_t data_sec_bm_len; // user data sector bitmap len 
    uf_uint8_t *data_sec_bm; // user data flash sector bitmap buf,when one bit is 1 then the sector is used

    uf_uint16_t find_start_sector;//where to find free sector from
    uf_uint16_t find_swap_start_sector; // cycle to write the swap block

    UF_SEC_GATHER_S mag_sec_gather;
    UF_SEC_GATHER_S swap_sec_gather;
    UF_SEC_GATHER_S data_sec_gather;
}UF_FLASH_DESC_S;


typedef struct {
    void *(*malloc_fn)(size_t sz);
    void (*free_fn)(void *ptr);
}sf_hook_s;


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: uf_write
*  description: file write
*  Input: uf_flash_desc uf_hand buf len 
*  Output: write_len
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_write(UF_FLASH_DESC_S *uf_flash_desc, UF_HAND_S *uf_hand,uf_uint8_t *buf,uf_uint32_t len,uf_uint32_t *write_len);

__UF_FLASH_FILE_EXT \
uf_ret uf_data_area_write_direct(UF_FLASH_DESC_S *uf_flash_desc, UF_HAND_S *uf_hand,uf_uint8_t *buf,uf_uint32_t len,uf_uint32_t *write_len);


/***********************************************************
*  Function: uf_read
*  description: file read
*  Input: uf_flash_desc uf_hand buf len 
*  Output: read_len
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_read(UF_FLASH_DESC_S *uf_flash_desc, UF_HAND_S *uf_hand,uf_uint8_t *buf,uf_uint32_t len,uf_uint32_t *read_len);


__UF_FLASH_FILE_EXT \
uf_ret uf_data_area_read(UF_FLASH_DESC_S *uf_flash_desc, UF_HAND_S *uf_hand,uf_uint8_t *buf,uf_uint32_t len,uf_uint32_t *read_len);

/***********************************************************
*  Function: uf_init
*  description: uf flash file init
*  Input: uf_desc uf_cfg
*  Output: none
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_init(UF_FLASH_DESC_S **uf_desc, UF_FLASH_CFG_S *uf_cfg);

/***********************************************************
*  Function: uf_uninit
*  description: uf flash file uninit
*  Input: uf_flash_desc
*  Output: none
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
void uf_uninit(UF_FLASH_DESC_S **uf_flash_desc);

/***********************************************************
*  Function: uf_open
*  description: uf file open
*  Input: uf_flash_desc pathname mode
*  Output: fd
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_open(UF_FLASH_DESC_S *uf_flash_desc,const uf_uint8_t *pathname,const file_mode_t mode,UF_HAND_S **fd);

/***********************************************************
*  Function: uf_close
*  description: uf file close
*  Input: fd
*  Output: none
*  Return: none
***********************************************************/
__UF_FLASH_FILE_EXT \
void uf_close(UF_HAND_S **fd);

/***********************************************************
*  Function: uf_delete
*  description: uf file delete
*  Input: uf_flash_desc filepath
*  Output: none
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_delete(UF_FLASH_DESC_S *uf_flash_desc,uf_uint8_t *filepath);

/***********************************************************
*  Function: uf_get_size
*  description: uf file delete
*  Input: uf_flash_desc filepath
*  Output: size
*  Return: uf_ret
***********************************************************/
__UF_FLASH_FILE_EXT \
uf_ret uf_get_size(UF_FLASH_DESC_S *uf_flash_desc,uf_uint8_t *filepath,uf_uint32_t *size);


#ifdef __cplusplus
}
#endif
#endif
