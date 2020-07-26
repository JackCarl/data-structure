//#include "LKT.h"

#ifndef __DES_h
#define __DES_h
#endif



#ifdef 	__DES_h


unsigned char Char8ToBit64(unsigned char *ch,unsigned char *bit_a); 
void Bit64ToChar8(unsigned char bit_a[64],unsigned char ch[8]);
void DES_PC1_Transfrom(unsigned char key[64],unsigned char tempbts[56]); 
void DES_PC2_Transfrom(unsigned char key[56],unsigned char tempbts[48]);  
void DES_ROL(unsigned char *data_a,unsigned char time);
void DES_ROR(unsigned char *data_a,unsigned char time);
void DES_IP_Transform(unsigned char data_a[64]);
void DES_IP_1_Transform(unsigned char data_a[64]);
void DES_E_Transform(unsigned char data_a[48]); 
void DES_P_Transform(unsigned char data_a[32]);
void DES_SBOX(unsigned char data_a[48]);
void DES_XOR(unsigned char R[48],unsigned char L[48],unsigned char count);
void DES_Swap(unsigned char left[32],unsigned char right[32]);
void encrypt_des(unsigned char *inoutdata,unsigned char *keyStr); 
void decrypt_des(unsigned char *inoutdata, unsigned char *keyStr);

void CbcDes_encrypt(unsigned char *iv,unsigned char *MsgIn, unsigned char *Key);
void CbcDes_decrypt(unsigned char *iv,unsigned char *MsgIn, unsigned char *Key);

void encrypt_3des(unsigned char *inoutdata,unsigned char *keyStr);
void decrypt_3des(unsigned char *inoutdata ,unsigned char *keyStr);

int Des_encrypt(unsigned char LenofKey, unsigned char LenofIn,unsigned char *pKey,unsigned char *pIn,unsigned char *pOut);
int Des_decrypt(unsigned char LenOfKey, unsigned char LenOfIn, unsigned char * pKey, unsigned char * pIn, unsigned char * pOut );

int CBC_3DES_Decrypt(unsigned char *RND1,unsigned char *InData,unsigned char *OutData,unsigned char *Key);

/*CBC_DES加密函数*/
/* iv 初始向量8字节；    inoutdata 输入时为明文数据，输出时为密文数据（注：第一字节为长度）；   keyStr  加密密钥   */
void Cbcencrypt_3des(unsigned char *iv,unsigned char *inoutdata,unsigned char *keyStr);

/*CBC_DES解密函数*/
void Cbcdecrypt_3des(unsigned char *iv,unsigned char *inoutdata ,unsigned char *keyStr);
/*iv 初始向量8字节；    inoutdata 输入时为密文数据，输出时为明文数据（注：第一字节为长度）；   keyStr  加密密钥   */

#endif

