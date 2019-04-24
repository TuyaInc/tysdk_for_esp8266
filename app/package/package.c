/***********************************************************
*  File: package.c 
*  Author: nzy
*  Date: 20150915
***********************************************************/
#define __PACKAGE_GLOBALS
#include "package.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <sys/types.h> // open()
#include <fcntl.h>  
#include <unistd.h>
#include <errno.h>
#include "com_def.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define DEBUG 0

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
// argv[0]:exe name
// argv[argc-1]:package file name
int main(int argc,char *argv[]) 
{
    if(argc < 3) {
        printf("the input parameter is not enough\n");
        return 1;
    }

    unsigned int ug_pkg_desc_len = sizeof(UG_PKG_FH_S) + ((argc-2)*sizeof(UG_FILE_S)) + \
                                   sizeof(UG_PKG_TAIL_S);
    UG_PKG_FH_S *pkg_fh = malloc(ug_pkg_desc_len);
    if(NULL == pkg_fh) {
        printf("malloc error\n");
        return 2;
    }
    memset(pkg_fh,0,ug_pkg_desc_len);

    pkg_fh->head = HTONL(UG_PKG_HEAD);
    strcpy((char *)pkg_fh->sw_ver,SW_VER);
    pkg_fh->ugf_cnt = HTONL(argc-2);
    
    int fd = 0;
    int ug_pkg_fd = open(argv[argc-1],O_CREAT | O_TRUNC | O_WRONLY, 0640);
    if(ug_pkg_fd < 0) {
        printf("open error line:%d.\n",__LINE__);
        goto ERR_EXIT;
    }

    int i = 0;    
    int j = 0;
    unsigned int sum = 0;
    int ret = 0;
    
    #define BUF_SIZE 4096
    unsigned char buf[BUF_SIZE];
    
    for(i = 0;i < argc-2;i++) {
        fd = open(argv[1+i],O_RDONLY);
        if(fd < 0) {
            printf("open error line:%d.\n",__LINE__);
            goto ERR_EXIT;
        }

        unsigned file_size = lseek( fd, 0, SEEK_END );
        if(file_size <= 0) {            
            printf("lseek error line:%d.\n",__LINE__);
            goto ERR_EXIT;
        }
        #if DEBUG
        else {
            printf("%s file_size:%x\n",argv[1+i],file_size);
        }
        #endif

        pkg_fh->ugf[i].len = HTONL(file_size);
        if(0 == i) {
            pkg_fh->ugf[i].offset = HTONL(ug_pkg_desc_len);
        }else {
            pkg_fh->ugf[i].offset = HTONL(NTOHL(pkg_fh->ugf[i-1].offset)+NTOHL(pkg_fh->ugf[i-1].len));
        }
        
        lseek(fd,0,SEEK_SET);
        lseek(ug_pkg_fd,NTOHL(pkg_fh->ugf[i].offset),SEEK_SET);
        sum = 0;
        unsigned int read_size = 0;
        while(read_size < file_size) {
            ret = read( fd, buf, BUF_SIZE);
            if(ret < 0) {
                printf("read error line:%d.\n",__LINE__);
                goto ERR_EXIT;
            }else if(0 == ret){ // 结尾
                break;
            }

            ret = write(ug_pkg_fd,buf,ret);
            if(ret < 0) {
                printf("write error line:%d.\n",__LINE__);
                goto ERR_EXIT;
            }

            for(j = 0;j < ret;j++) {
                sum += buf[j];
            }
            read_size += ret;
        }
        pkg_fh->ugf[i].sum = HTONL(sum);
        #if DEBUG
        printf("%s sum:%x\n",argv[1+i],sum);
        #endif
        close(fd),fd = 0;
    }

    sum = 0;
    unsigned char *tmp = (unsigned char *)pkg_fh;
    UG_PKG_TAIL_S *pkg_tail = (UG_PKG_TAIL_S *)(tmp + (sizeof(UG_PKG_FH_S) + (argc-2)*sizeof(UG_FILE_S)));
    for(j = 0;j < (ug_pkg_desc_len-sizeof(UG_PKG_TAIL_S));j++) {
        sum += tmp[j];
    }
    pkg_tail->sum = HTONL(sum);
    pkg_tail->tail = HTONL(UG_PKG_TAIL);
    #if DEBUG
    printf("sum:%x\n",sum);
    #endif
    
    lseek(ug_pkg_fd,0,SEEK_SET);
    ret = write(ug_pkg_fd,tmp,ug_pkg_desc_len);
    if(ret < 0) {
        printf("write error line:%d.\n",__LINE__);
        goto ERR_EXIT;
    }
    
    close(ug_pkg_fd),ug_pkg_fd = 0;
    free(pkg_fh),pkg_fh = NULL;

    return 0;
    
ERR_EXIT:
    if(pkg_fh) {
        free(pkg_fh);
    }

    if(ug_pkg_fd) {
        close(ug_pkg_fd);
    }

    if(fd) {
        close(fd);
    }

    return 3;
} 



















