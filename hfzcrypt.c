#include "crypto.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>

void kullanim_goster(void) {
    printf("Secenekler:\n");
    printf("  -e, --encrypt <dosya>    Belirtilen dosyayi sifreler.\n");
    printf("  -d, --decrypt <dosya>    Belirtilen dosyanin sifresini cozer.\n");
    printf("  -a, --algorithm <aes|chacha> \n");
    printf("  -h, --help               Bu yardim mesajini gosterir.\n");
    printf("\nOrnek kullanimlar:\n\n");
    printf("  hfzcrypt -e belge.pdf -a aes\n");
    printf("  hfzcrypt -e belge.pdf -a chacha\n");
    printf("  hfzcrypt -d belge.pdf.hfz\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int b = 1; 
    char *girdi_dosya = NULL;
    bool mode_encrypt = false;
    bool mode_decrypt = false;
    char *algoritma = NULL;

    static struct option long_option[] = {
        {"encrypt",   required_argument, NULL, 'e'},
        {"decrypt",   required_argument, NULL, 'd'},
        {"algoritma", required_argument, NULL, 'a'},
        {"help",      no_argument,       NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "e:d:a:h", long_option, NULL)) != -1) {
        switch (opt) {
            case 'e':
                mode_encrypt = true;
                girdi_dosya = optarg; 
                break;
            case 'd':
                mode_decrypt = true;
                girdi_dosya = optarg;
                break;
            case 'a':
                algoritma = optarg;
                break;
            case 'h':
                kullanim_goster();
                return 0;
            case '?':
                kullanim_goster();
                return 1;
            default:
                abort();
        }
    }

    if (!mode_encrypt && !mode_decrypt) {
        fprintf(stderr, "[HATA] Bir islem secilmelidir (-e veya -d).\n");
        kullanim_goster();
        return 1;
    }

    if (mode_encrypt && mode_decrypt) {
        fprintf(stderr, "[HATA] Ayni anda hem sifreleme hem cozme yapilamaz!\n");
        return 1;
    }

    if (mode_encrypt && (girdi_dosya == NULL || algoritma == NULL)) {
        fprintf(stderr, "[HATA] Dosya adi ve algoritma zorunludur.\n");
        return 1;
    }

    if (mode_decrypt && girdi_dosya == NULL) {
        fprintf(stderr, "[HATA] Dosya adi zorunludur.\n");
        return 1;
    }

    if (mode_encrypt) {
        if (strcmp(algoritma, "aes") == 0)
            b = 1;
        else if (strcmp(algoritma, "chacha") == 0)
            b = 2;
        else {
            fprintf(stderr, "Gecersiz algoritma.\n");
            return 1;
        }
    }

  
    struct termios eski, yeni;
    tcgetattr(STDIN_FILENO, &eski);
    yeni = eski;
    yeni.c_lflag &= ~ECHO;  
    tcsetattr(STDIN_FILENO, TCSANOW, &yeni);

    printf("HFZCrypt v1.0\n");
    printf("Made by h4fizA\n\n");
    char parola[256];
    printf("Parola: ");
    if (fgets(parola, sizeof(parola), stdin) == NULL) {
        tcsetattr(STDIN_FILENO, TCSANOW, &eski);
        return 1;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &eski);
    printf("\n");
    parola[strcspn(parola, "\n")] = '\0';


    if (mode_encrypt) {
        if (sifrele(parola, girdi_dosya, b))
            printf("[+] Sifreleme basarili.\n");
        else
            printf("[-] Sifreleme basarisiz.\n");
    }
    else if (mode_decrypt) {
        if (coz(parola, girdi_dosya))
            printf("[+] Cozme basarili.\n");
        else
            printf("[-] Cozme basarisiz.\n");
    }

    return 0;
}