#ifndef _UNI_MD5_H
#define _UNI_MD5_H

typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
{ \
    a += F(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
    a += G(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
    a += H(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
    a += I(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
void md5_init(MD5_CTX *context);
void md5_update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void md5_final(MD5_CTX *context,unsigned char digest[16]);
void md5_transform(unsigned int state[4],unsigned char block[64]);
void md5_encode(unsigned char *output,unsigned int *input,unsigned int len);
void md5_decode(unsigned int *output,unsigned char *input,unsigned int len);


typedef MD5_CTX UNI_MD5_CTX_S;
#define  uni_md5_init        md5_init
#define  uni_md5_update      md5_update
#define  uni_md5_final       md5_final





#endif
