#ifndef TUYA_TLS_H
#define TUYA_TLS_H

/* mbedtls 仅仅用于通道加密通讯，不再用于通道建立。*/

#include "tuya_cloud_types.h"

typedef PVOID_T tuya_tls_hander;

typedef INT_T (*tuya_tls_send_cb)( VOID *p_custom_net_ctx, CONST BYTE_T *buf, SIZE_T len);
typedef INT_T (*tuya_tls_recv_cb)( VOID *p_custom_net_ctx, BYTE_T *buf, SIZE_T len);
VOID tuya_tls_register_constant(IN CHAR_T *p_uuid, IN CHAR_T *p_authkey);

VOID tuya_tls_set_ssl_verify(IN CONST INT_T verify_mode);
INT_T tuya_tls_register_x509_crt_der(VOID *p_ctx, UCHAR_T *p_der, UINT_T der_len);

VOID tuya_tls_set_load_cert(BOOL_T load);
OPERATE_RET tuya_tls_connect(OUT tuya_tls_hander *p_tls_handler, IN CHAR_T *hostname,IN INT_T port_num,
                             IN VOID *p_custom_net_ctx, IN tuya_tls_send_cb send_cb, IN tuya_tls_recv_cb recv_cb,
                             IN INT_T socket_fd, IN INT_T overtime_s);

INT_T tuya_tls_write(IN tuya_tls_hander tls_handler, BYTE_T *buf, UINT_T len);
INT_T tuya_tls_read(IN tuya_tls_hander tls_handler, BYTE_T *buf, UINT_T len);
OPERATE_RET tuya_tls_disconnect(IN tuya_tls_hander tls_handler);




#endif//TUYA_TLS_H


