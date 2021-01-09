/***********************************************************
*  File: simple_flash.h 
*  Author: nzy
*  Date: 20160804
***********************************************************/
#ifndef __SIMPLE_FLASH_H
	#define __SIMPLE_FLASH_H

    #include "stdlib.h"
    #include "string.h"
    #include "stdio.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __SIMPLE_FLASH_GLOBALS
	#define __SIMPLE_FLASH_EXT
#else
	#define __SIMPLE_FLASH_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

#define SIMPLE_FLASH_ENCRYPT_ENABLE 1

typedef unsigned char sf_char_t;

typedef signed char sf_int8_t;
typedef unsigned char sf_uint8_t;

typedef short sf_int16_t;
typedef unsigned short sf_uint16_t;

typedef signed int sf_int32_t;
typedef unsigned int sf_uint32_t;

typedef sf_int32_t sf_ret;
#define SF_OK 0
#define SF_INVALID_PARM 1
#define SF_MALLOC_ERR 2
#define SF_COM_ERR 3
#define SF_NO_FIND 4
#define SF_NOT_ENOUGH_PAGE 5
#define SF_BLOCK_UNVALID 6
#define SF_CRC_ERR 7
#define SF_PG_SIZE_NOT_ENOUGH 8
#define SF_CHECK_ERR_WHEN_WRITE 9
#define SF_GET_FLASH_KEY_ERR 10


typedef sf_ret (*flash_read_f)(sf_uint32_t addr, sf_uint8_t *dst, sf_uint32_t size);
typedef sf_ret (*flash_write_f)(sf_uint32_t addr, sf_uint8_t *src, sf_uint32_t size);
typedef sf_ret (*flash_erase_f)(sf_uint32_t addr, sf_uint32_t size);
typedef sf_uint32_t (*crc32_f)(void *data,sf_uint16_t len);

struct s_sf_index;
#define INDEX_CACHE_LMT 32 // 32
#define AES_ENCRYPT_KEY_LEN 16
typedef struct {
    sf_uint32_t start_addr; // user physical flash start address 
    sf_uint32_t flash_sz; // user flash size
    sf_uint32_t block_sz; // flash block size
    sf_uint32_t swap_start_addr; // swap flash start address
    sf_uint32_t swap_flash_sz; // swap flash size
    sf_uint32_t flash_key_addr;//flash encrypt key restore addr
    flash_read_f sf_read;
    flash_write_f sf_write;
    flash_erase_f sf_erase;
    crc32_f sf_crc32;
    sf_uint8_t encrypt_key[AES_ENCRYPT_KEY_LEN];
}sf_flash_cfg_s;

typedef struct {
    sf_flash_cfg_s cfg;
    sf_uint16_t page_sz; // logic page size
    sf_uint16_t block_pages; // numbers of pages in one block

    sf_uint16_t block_nums; // flash block numbers
    sf_uint16_t swap_block_nums; // swap block numbers
    sf_uint16_t swap_blk_idx; // cycle to write the swap block
    
    sf_uint32_t flash_pages; // numbers of pages in all flash
    sf_uint32_t free_pages; // free page number
    sf_uint32_t alloc_gpid_index; // use to write flash block circularly

    sf_uint16_t pg_bm_len; // user flash bitmap len 
    sf_uint8_t *pg_bm; // user flash bitmap buf,when one bit is 1 then the page is used

    sf_uint16_t sf_index_cnt;
    struct s_sf_index *idx_cache[INDEX_CACHE_LMT];
    sf_uint8_t cache_cnt; // cache count
    sf_uint8_t up_index; // cache update index
}sf_hand_s;

#define DATA_NAME_LMT 32

// Each block is described below 
/*
block 0 sf block manage
block 1 sf_index_t page or data page
.
.
.
.
block x sf_data_t_magage
*/

#pragma pack (1)
#define SIMPLE_BF_MAGIC 0x98761234
typedef struct { // The the first page in each block
    sf_uint32_t magic;
    sf_uint32_t crc32; // block crc32,sizeof(block size)-sizeof(magic+crc32)
    sf_uint16_t block_id;
    sf_uint32_t wr_count; // write count
    
    sf_uint8_t pg_bm_len; // page bitmap len 
    sf_uint8_t pg_bm[0]; // page bitmap array,when one bit is 1 then the page is index object
}sf_block_mag_s;

typedef struct {
    sf_uint16_t block_id;
    sf_uint8_t start_page_id;
    sf_uint8_t end_page_id;
}sf_data_element_s;

typedef struct s_sf_index {
    sf_uint32_t crc32; // crc(data)
    sf_uint32_t data_len;
    sf_uint16_t block_id;
    sf_uint8_t bpg_id;
    sf_uint16_t element_num;
    sf_data_element_s *element; // need read more data from flash
    sf_uint8_t name_len;
    sf_uint8_t name[0];
}sf_index_s;

#define KEY_MAGIC 0x13579753
typedef struct {//struct of key block
    sf_uint32_t magic;
    sf_uint32_t crc32; // block crc32,sizeof(block size)-sizeof(magic+crc32)
    sf_uint8_t data[0];
}sf_block_key_s;

#pragma pack ()

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
*  Function: sf_init_hooks
*  description: register memory method
*  Input: hooks->memory method
*  Output: none
*  Return: none
***********************************************************/
__SIMPLE_FLASH_EXT \
void sf_init_hooks(sf_hook_s *hooks);

/***********************************************************
*  Function: create_flash_hand_and_init
*  description: 
*  Input: cfg-> simple flash config
*  Output: ohand-> simple flash hand
*  Return: sf_ret
*  Note: if(cfg->sf_crc32 == NULL) then "use sum32 to verify 
*        data validity internally" 
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret create_flash_hand_and_init(sf_hand_s **ohand,const sf_flash_cfg_s *cfg);

/***********************************************************
*  Function: delete_flash_hand
*  description: 
*  Input: hand->simple flash hand
*  Output: 
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
void delete_flash_hand(const sf_hand_s *hand);

/***********************************************************
*  Function: sf_write
*  description: simple flash write method
*  Input: hand->simple flash hand
*         name->data name
*         data->data
*         len->data len
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_write(sf_hand_s *hand,const sf_char_t *name,\
                const sf_uint8_t *data,const sf_uint32_t len);

/***********************************************************
*  Function: sf_read
*  description: simple flash read method
*  Input: hand->simple flash hand
*         name->data name
*         data->write data
*         len->data len
*  Output: data->read data,need call sf_mem_free to free
*          len->read data len 
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_read(sf_hand_s *hand,const sf_char_t *name,\
               sf_uint8_t **data,sf_uint32_t *len);

/***********************************************************
*  Function: sf_fuzzy_read
*  description: simple flash read method
*  Input: hand->simple flash hand
*         fuzzy_name->fuzzy_name data name
*         data->write data
*         len->data len
*         gpid->read the fuzzy name index start gpid
*  Output: data->read data,need call sf_mem_free to free
*          len->read data len
*          gpid->the find index gpid +1
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_fuzzy_read(sf_hand_s *hand,const sf_char_t *fuzzy_name,sf_uint32_t *gpid,\
                     sf_uint8_t **data,sf_uint32_t *len);


/***********************************************************
*  Function: sf_format
*  description: simple flash format
*  Input: hand
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_format(sf_hand_s *hand);

/***********************************************************
*  Function: sf_delete
*  description: simple flash data delete method
*  Input: hand->simple flash hand
*         name->delete data name
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_delete(sf_hand_s *hand,const sf_char_t *name);

/***********************************************************
*  Function: sf_fuzzy_delete
*  description: simple flash data delete method
*  Input: hand->simple flash hand
*         fuzzy_name->delete data name
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_EXT \
sf_ret sf_fuzzy_delete(sf_hand_s *hand,const sf_char_t *fuzzy_name);

/***********************************************************
*  Function: sf_mem_free
*  description: simple flash memory free
*  Input: mem->memory to free
*  Output: none
*  Return: sf_ret
*  Note: "sf_read" output data maybe use this method to free 
*        data memory
***********************************************************/
__SIMPLE_FLASH_EXT \
void sf_mem_free(void *mem);

#ifdef __cplusplus
}
#endif
#endif


