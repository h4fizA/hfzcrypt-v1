#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <stdint.h>
#include <stdbool.h>
typedef struct
{
    char magic[8];
    uint8_t algoritma;
    uint8_t tag[16];
    uint8_t iv[12];
    uint8_t salt[16];
} __attribute__((packed)) Paket;
void open_ssl_hata(char *mesaj);
int generate_key(uint8_t key[32],
                  const uint8_t salt[16],
                  const char *password);

bool sifrele(char *password, const char *filename, uint8_t secim);
bool coz(const char *password, const char *file_name);


#endif