/***********************************************************
*  File: base64.h
*  Author: nzy
*  Date: 20150609
***********************************************************/
#ifndef _UNI_BASE64_H
    #define _UNI_BASE64_H

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __BASE64_GLOBALS
    #define __BASE64_EXT
#else
    #define __BASE64_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
__BASE64_EXT \
char * base64_encode( const unsigned char * bindata, char * base64, int binlength );

__BASE64_EXT \
int base64_decode( const char * base64, unsigned char * bindata );

#ifdef __cplusplus
}
#endif
#endif

