#ifndef _AES_INF_H_
#define _AES_INF_H_

#if 0
#include <stdint.h>
#else
#include "espressif/c_types.h"
#endif

#include "error_code.h"
#include "tuya_cloud_types.h"


void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);


void AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);


OPERATE_RET aes128_ecb_encode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              OUT BYTE_T **ec_data,OUT UINT_T *ec_len,\
                              IN CONST BYTE_T *key);

OPERATE_RET aes128_ecb_decode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              OUT BYTE_T **dec_data,OUT UINT_T *dec_len,\
                              IN CONST BYTE_T *key);

INT_T aes128_get_data_actual_length(IN CONST BYTE_T *dec_data,IN CONST UINT_T dec_data_len);

OPERATE_RET aes128_free_data(IN BYTE_T *data);

#endif //_AES_INF_H_
