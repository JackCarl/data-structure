#include <fstream>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <cstdio>
#include <cerrno>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <list>
#include <signal.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "des.h" 

static  bool Running = true;
void handler(int value) {
    printf("Deal with signal[%d][%s %d]\n", value, __FILE__, __LINE__);
    Running = false;
}

static void signal_init(void (*handler)(int signal)) {
    struct sigaction NewAction;
    sigemptyset(&NewAction.sa_mask);
    NewAction.sa_flags = 0;
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGABRT, &NewAction, NULL);
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGFPE, &NewAction, NULL);
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &NewAction, NULL);
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &NewAction, NULL);
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &NewAction, NULL);
    NewAction.sa_handler = SIG_IGN;
    sigaction(SIGUSR2, &NewAction, NULL);
    NewAction.sa_handler = handler;
    sigaction(SIGINT, &NewAction, NULL);
    NewAction.sa_handler = handler;
    sigaction(SIGQUIT, &NewAction, NULL);
    NewAction.sa_handler = handler;
    sigaction(SIGTERM, &NewAction, NULL);
    NewAction.sa_handler = handler;
    sigaction(SIGTTIN, &NewAction, NULL);
    NewAction.sa_handler = handler;
    sigaction(SIGTTOU, &NewAction, NULL);
}

#define LOG(fmt, args...) do { printf(fmt, ##args); printf("\n"); } while (0)
#define EXIT(fmt, args...) do { LOG(fmt, ##args); exit(0); } while (0)

static size_t load_file(unsigned char **buf, const char *filepath)
{
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL)
        EXIT("load_file, %s open failed", filepath);
    
    fseek(fp, 0L, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (*buf == NULL)
        *buf = (unsigned char *)malloc(sizeof(unsigned char) * size);
    if (*buf == NULL)
        EXIT("system error");
    
    size_t read_size = fread(*buf, sizeof(unsigned char), size, fp);

    if (read_size != size)
        EXIT("read file error");
    
    fclose(fp);
    fp = NULL;
    return size;
}

static void save_file(const unsigned char *data, size_t size, const char *filepath)
{
    FILE *fp = fopen(filepath, "w");
    if (fp == NULL)
        EXIT("save_file,%s open failed", filepath);
    
    if (data == NULL)
        EXIT("write data is empty");

    size_t write_size = fwrite(data, sizeof(unsigned char), size, fp);
    
    if (size != write_size)
        EXIT("write file error");
    
    fflush(fp);
    fclose(fp);
    fp = NULL;
}

void init_crypto(unsigned int encbits, size_t datasize, unsigned int *nblocks, size_t *bsize)
{
    *bsize = encbits / 8;
    *nblocks = (datasize % (*bsize) == 0) ? (datasize / (*bsize)) : (datasize / (*bsize) + 1);
}

static unsigned char *process(int enc, const char *inpath, const char *outpath, const char *userkey_path)
{
    unsigned char *buf = NULL;
    unsigned char *userkey = NULL;

	unsigned char iv[8] = {0x12,0x43,0x23,0x32,0x31,0x54,0x32,0x65};
	
    size_t bsize;
    unsigned int nblocks;
//    unsigned int encbit = BOOTIMG_CRYPTO_ENCRYPT_BITS;
	unsigned int encbit = 256;

    size_t datasize = load_file(&buf, inpath);
    size_t userkey_size = load_file(&userkey, userkey_path);

    init_crypto(encbit, datasize, &nblocks, &bsize);
    printf("datasize = %ld nblocks = %d, bsize = %ld\r\n",datasize, nblocks, bsize);
	
    if (datasize == 0 || userkey_size == 0)
        EXIT("data or userkey empty");

	unsigned char *inputbuf = (unsigned char *)malloc(bsize + 1);
	if (inputbuf == NULL)
        EXIT("system error");
#if 0
	for(int i = 0; i < nblocks - 1; i++){	//Cbcencrypt_3des加密数据流长度必须是size倍数,最后一包不加密
		inputbuf[0] = bsize;
		memcpy(inputbuf + 1, buf + bsize * i, bsize);
		if(enc){
			Cbcencrypt_3des(iv,inputbuf,userkey);
		}else{
			Cbcdecrypt_3des(iv,inputbuf,userkey);
		}
		memcpy(buf + bsize * i, inputbuf + 1, bsize);
	}
#else 
#define STARTOFFSET 0x840
#define OFFSET   1024
	for(int i = 0; i < 10; i++){	
		inputbuf[0] = bsize;
		memcpy(inputbuf + 1, buf + STARTOFFSET + i * OFFSET, bsize);
		if(enc){
			Cbcencrypt_3des(iv,inputbuf,userkey);
		}else{
			Cbcdecrypt_3des(iv,inputbuf,userkey);
		}
		memcpy(buf + STARTOFFSET + i * OFFSET, inputbuf + 1, bsize);
	}	
#endif
	if(inputbuf){
		free(inputbuf);
	}
/*
    if (enc) {
        bootimg_set_size_into_hdr((boot_img_hdr *)buf, nblocks * bsize);
        bootimg_set_flag_into_hdr((boot_img_hdr *)buf, FIRST_BOOT_FLAG);
    }

    do_crypto(enc, userkey, encbit, buf, datasize, nblocks);
*/    
	if(userkey){
		free(userkey); 
		userkey = NULL;
	}
   
    if (outpath) {
		save_file(buf, datasize, outpath);
    }
    return buf;
}


int main(int argc, char **argv) {
	std::cout <<"---program start---"<< std::endl;

	if (argc == 1) {
        LOG("Useage: ./bootimg_factory [-en][-de] [in file] [out file] [key file]");
        LOG("Useage: ./bootimg_factory [-md5] [in file]");
        return -1;
    }

    const char *option = argv[1];
    const char *inpath = argv[2];
    const char *outpath = argc == 5 ? argv[3] : NULL;
    const char *keypath = argc == 5 ? argv[4] : NULL;

    unsigned char *outdata = NULL;

    if (strcmp(option, "-en") == 0) {
		outdata = process(1, inpath, outpath, keypath);
    } else if (strcmp(option, "-de") == 0) {
        outdata = process(0, inpath, outpath, keypath);
    } else if (strcmp(option, "-md5") == 0) {
        //append_end_flag(inpath);
//        md5lk(inpath);
    } else {
        LOG("bad param");
    }

    if (outdata) {
        free(outdata);
		outdata = NULL;
    }
    
	std::cout <<"---program stop---"<< std::endl;
    return 0;
}




