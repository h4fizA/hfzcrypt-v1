#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAGIC "hfz1638!"
Paket *header(uint8_t *iv,uint8_t *tag,uint8_t *salt,uint8_t secim){
    Paket *ptk = malloc(sizeof(Paket));


if (ptk == NULL)
{
    return NULL;
}
    memcpy(ptk->magic, MAGIC, sizeof(ptk->magic));
    memcpy(ptk->tag,tag,16);
    memcpy(ptk->salt,salt,16);
    memcpy(ptk->iv,iv,12);
    ptk->algoritma=secim;
    return ptk;
}
void open_ssl_hata(char *mesaj)
{
    fprintf(stderr, "%s\n", mesaj);
    ERR_print_errors_fp(stderr);
}
int generate_key(uint8_t key[32],const uint8_t salt[16],const char *password){
                    int len=strlen(password);
                    if(PKCS5_PBKDF2_HMAC(password,len,salt,16,600000,EVP_sha256(),32,key)!=1){
                        open_ssl_hata("sifre uretilemedi");
                        return 0 ;
                    }
                return 1;
                  }
bool sifrele(char *password,const char *filename,uint8_t secim){
    FILE *fp=NULL;
    FILE *fp2=NULL;
    char uzanti[]=".hfz";
    char new_filename[256];
    int len=0;
    int cipher_len;
    uint8_t buffer[1024*1024];
    uint8_t cipher_buffer[1024*1024];
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
 
if (ctx == NULL)
{
    open_ssl_hata("CTX oluşturulamadı.");
    return false;
}
    const EVP_CIPHER *cipher;

switch (secim)
{
case 1:
    cipher = EVP_aes_256_gcm();
    break;

case 2:
    cipher = EVP_chacha20_poly1305();
    break;

default:
    fprintf(stderr, "Geçersiz algoritma.\n");
    EVP_CIPHER_CTX_free(ctx);
    return false;
}

    uint8_t key[32];
    uint8_t salt[16];
if (RAND_bytes(salt,16)!=1){
    open_ssl_hata("salt olusturulamadi.");
    EVP_CIPHER_CTX_free(ctx);
    return false;
}
if (!generate_key(key, salt, password))
{
    EVP_CIPHER_CTX_free(ctx);
    return false;
}
    uint8_t iv[12];

   if (RAND_bytes(iv, sizeof(iv)) != 1)
{
    open_ssl_hata("IV oluşturulamadı.");
    EVP_CIPHER_CTX_free(ctx);
    return false;
}
    
  if (snprintf(new_filename,
             sizeof(new_filename),
             "%s%s",
             filename,
             uzanti) >= sizeof(new_filename))
{
    fprintf(stderr, "Dosya adı çok uzun.\n");
    EVP_CIPHER_CTX_free(ctx);
    return false;
}
    if(EVP_EncryptInit_ex(ctx,cipher,NULL,key,iv)!=1){
        open_ssl_hata("sifreleme hatasi");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
   fp = fopen(filename, "rb");
    if (fp == NULL){
    fprintf(stderr, "Dosya açılamadı.\n");
    EVP_CIPHER_CTX_free(ctx);
    return false;}
    fp2=fopen(new_filename,"wb");
    if(fp2==NULL){
    fprintf(stderr, "sifreli dosya acilamadi\n");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);;
    return false;}
Paket bos = {0};

if (fwrite(&bos, sizeof(Paket), 1, fp2) != 1)
{
    fprintf(stderr, "Header ayrilamadi.\n");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    return false;
}
    while ((len=fread(buffer,1,1024*1024,fp))>0)
    {
        if(EVP_EncryptUpdate(ctx,cipher_buffer,&cipher_len,buffer,len)!=1){
        open_ssl_hata("Şifreleme hatası");
        EVP_CIPHER_CTX_free(ctx);
        fclose(fp);
        fclose(fp2);
        return false;
        }
         if (fwrite(cipher_buffer, 1, cipher_len, fp2) != (size_t)cipher_len)
{
    fprintf(stderr, "Dosyaya yazma hatası.\n");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    return false;
}

    }
    int final_len;

    if (EVP_EncryptFinal_ex(ctx, cipher_buffer, &final_len) != 1)
    {
             open_ssl_hata("Şifreleme hatası");
        EVP_CIPHER_CTX_free(ctx);
        fclose(fp);
        fclose(fp2);
        return false;
    }
    uint8_t tag[16];
    
    if (fwrite(cipher_buffer, 1, final_len, fp2) != (size_t)final_len)
{
    fprintf(stderr, "Final verisi yazılamadı.\n");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    
    return false;
}
if (EVP_CIPHER_CTX_ctrl(ctx,
                        EVP_CTRL_AEAD_GET_TAG,
                        16,
                        tag) != 1)
{
    open_ssl_hata("Tag alınamadı.");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    return false;
}
    Paket *ptk=header(iv,tag,salt,secim);
    if (ptk == NULL)
{
    EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    return false;
}
    fseek(fp2, 0, SEEK_SET);
if (fwrite(ptk, sizeof(Paket), 1, fp2) != 1)
{
    fprintf(stderr, "Header yazılamadı.\n");
  EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    free(ptk);
    return false;
}
   EVP_CIPHER_CTX_free(ctx);
    fclose(fp);
    fclose(fp2);
    free(ptk);
    OPENSSL_cleanse(key, sizeof(key));
OPENSSL_cleanse(password, sizeof(password));
    return true;
    }
bool coz(const char *password,const char *file_name){
    uint8_t cipher_buffer[1024 * 1024];
    const EVP_CIPHER *cipher;
    uint8_t plain_buffer[1024 * 1024];
    int len;
    int plain_len;
    char yeni_uzanti[256];
    uint8_t key[32];
    uint8_t tag[16];
    uint8_t iv[12];
    uint8_t salt[16];
    FILE *fp=NULL;
    FILE *fp1=NULL;
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
if (ctx == NULL)
{
    fprintf(stderr,"CTX oluşturulamadı.\n");
    return false;
}


    Paket ptk;


size_t len1 = strlen(file_name);
const char *ext = ".hfz";
size_t ext_len = strlen(ext);


if (len1 < ext_len || strcmp(file_name + len1 - ext_len, ext) != 0) {
    fprintf(stderr, "[HATA] Gecersiz dosya uzantisi. Sadece '.hfz' dosyalari cozulebilir: %s\n", file_name);
    return false;
}


snprintf(yeni_uzanti, sizeof(yeni_uzanti), "%s", file_name);


yeni_uzanti[len1 - ext_len] = '\0';
fp = fopen(file_name, "rb");

if (fp == NULL)
{
    fprintf(stderr, "Dosya açılamadı.\n");
    return false;
}
if (fread(&ptk, sizeof(Paket), 1, fp) != 1)
{
    fprintf(stderr, "Header okunamadı.\n");
    fclose(fp);
    return false;
}

if (memcmp(ptk.magic, MAGIC, sizeof(ptk.magic)) != 0)
{
    fprintf(stderr, "Magic eşleşmedi.\n");
    fclose(fp);
    return false;
}
switch (ptk.algoritma)
{
case 1:
    cipher = EVP_aes_256_gcm();
    break;

case 2:
    cipher = EVP_chacha20_poly1305();
    break;

default:
    fprintf(stderr, "Geçersiz algoritma.\n");
    EVP_CIPHER_CTX_free(ctx);
    return false;
}

    memcpy(salt,ptk.salt,16);
    memcpy(tag,ptk.tag,16);
    memcpy(iv,ptk.iv,12);
    if(!generate_key(key,salt,password)){
        open_ssl_hata("alinan paroladan sifre uretiilemedi.....");
        fclose(fp);
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    fp1 = fopen(yeni_uzanti, "wb");
    if (fp1 == NULL)
{
    fprintf(stderr, "Çıktı dosyası oluşturulamadı.\n");
    fclose(fp);
    EVP_CIPHER_CTX_free(ctx);
    return false;
}
    if(EVP_DecryptInit_ex(ctx,
                   cipher,
                   NULL,
                   key,
                   iv)!=1){
                    open_ssl_hata("decrypt hatasi");
                    fclose(fp);
                    fclose(fp1);
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                   }
                   if(EVP_CIPHER_CTX_ctrl(ctx,
                    EVP_CTRL_AEAD_SET_TAG,
                    16,
                    tag)!=1){
                        open_ssl_hata("tagda hata var");
                              fclose(fp);
                    fclose(fp1);
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                    }
    while ((len = fread(cipher_buffer, 1, sizeof(cipher_buffer), fp)) > 0)
{
    if(EVP_DecryptUpdate(ctx,
                  plain_buffer,
                  &plain_len,
                  cipher_buffer,
                  len)!=1){
                    open_ssl_hata("decrypt hatasi");
                    fclose(fp);
                    fclose(fp1);
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
                  }

    if(fwrite(plain_buffer,1,plain_len,fp1)!=plain_len){
         open_ssl_hata("decryptte yazma hatasi");
                    fclose(fp);
                    fclose(fp1);
                    EVP_CIPHER_CTX_free(ctx);
                    return false;
    }
}
int final_len;
if(EVP_DecryptFinal_ex(ctx,plain_buffer,&final_len)!=1){
  open_ssl_hata("decrypt hatasi");
                    fclose(fp);
                    fclose(fp1);
                    EVP_CIPHER_CTX_free(ctx);
                     remove(yeni_uzanti); 
                    return false;

}
if(fwrite(plain_buffer,1,final_len,fp1)!=(size_t)(final_len)){
       fprintf(stderr, "Final verisi yazılamadı.\n");

    fclose(fp);
    fclose(fp1);
    EVP_CIPHER_CTX_free(ctx);

    remove(yeni_uzanti);
    OPENSSL_cleanse(key, sizeof(key));
OPENSSL_cleanse(password, sizeof(password));
    return false;
}
EVP_CIPHER_CTX_free(ctx);
fclose(fp);
fclose(fp1);
remove(file_name);
return true;
}