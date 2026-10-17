# HFZCrypt - Dosya Şifreleme Aracı

HFZCrypt, C dilinde yazılmış ve OpenSSL 3.0+ EVP API altyapısını kullanan komut satırı tabanlı bir dosya şifreleme aracıdır.

AES-256-GCM ve ChaCha20-Poly1305 AEAD algoritmalarını destekler. Büyük dosyaları 1 MB'lık parçalar halinde akış (streaming) yöntemiyle işlediği için sistemi yormaz ve minimum bellek tüketir.

---

## Özellikler

- **Çift Algoritma Desteği:** Donanım destekli AES-256-GCM veya ChaCha20-Poly1305.
- **Anahtar Türetimi (KDF):** Parolalardan anahtar üretimi için 600.000 iterasyonlu PBKDF2-HMAC-SHA256 kullanılır.
- **Rastgele Değerler:** Salt (16 bayt) ve IV/Nonce (12 bayt) değerleri OpenSSL RAND_bytes ile üretilir.
- **Bütünlük Doğrulaması:** 16 baytlık Authentication Tag doğrulaması yapılır; parola yanlışsa ya da dosya kurcalanmışsa işlem hemen durdurulur ve dosya açılmaz.
- **Düşük Bellek Kullanımı:** Dosyaları 1 MB sabit tamponla parça parça okuyup yazar, gigabaytlarca büyüklükteki dosyalarda RAM tüketimi sabit kalır.
- **Format Doğrulaması:** Dosya başında hfz1638! sihirli bayt kontrolü bulunur.
- **Güvenli Parola Girişi:** Parola yazılırken termios ile ekranda gizlenir.
- **Argüman Yönetimi:** Standart getopt_long yapısı kullanılır.

---

## Performans Testi (1 GB Dosya)

Aşağıdaki ölçümler 1 GB boyutunda rastgele bir test dosyasıyla alınmıştır:

| Algoritma | Şifreleme | Şifre Çözme | Ortalama Hız |
|---|---|---|---|
| AES-256-GCM | ~0.94 sn | ~1.01 sn | ~1.05 GB/sn |
| ChaCha20-Poly1305 | ~1.04 sn | ~1.28 sn | ~0.96 GB/sn |

*Not: AES-NI komut setini destekleyen işlemcilerde donanımsal hızlandırma sayesinde AES-256-GCM ile 1 GB dosya 1 saniyenin altında işlenir.*

---

## Dosya Başlık Formatı (.hfz)

Şifrelenen her dosyanın başına 53 baytlık sabit bir başlık eklenir:

+-------------------+----------------+---------------+---------------+---------------+----------------------+
| Magic Header (8B) | Algoritma (1B) | GCM/Tag (16B) |    IV (12B)   |   Salt (16B)  | Şifreli Veri (N B)   |
|   "hfz1638!"      | 1:AES / 2:CC20 | Auth Tag      |  Random Nonce | PBKDF2 Salt   | Encrypted Payload    |
+-------------------+----------------+---------------+---------------+---------------+----------------------+

---

## Derleme ve Kurulum

### Bağımlılıklar

Debian, Ubuntu veya Pardus / Parrot OS üzerinde derlemek için gereken paketler:

```bash
sudo apt update
sudo apt install build-essential libssl-dev
```

### Derleme

```bash
gcc -O3 hfzcrypt.c crypto.c -o hfzcrypt -lcrypto
```

---

## Kullanım

### Yardım

```bash
./hfzcrypt -h
```

### 1. Dosya Şifreleme (-e)

AES-256-GCM ile şifreleme:
```bash
./hfzcrypt -e belge.pdf -a aes
```
*Çıktı: belge.pdf.hfz*

ChaCha20-Poly1305 ile şifreleme:
```bash
./hfzcrypt -e video.mp4 -a chacha
```
*Çıktı: video.mp4.hfz*

### 2. Dosya Şifresini Çözme (-d)

```bash
./hfzcrypt -d belge.pdf.hfz
```
*Algoritma başlık bilgisinden otomatik algılanır ve orijinal dosya çıkarılır.*

---

## Dosya Yapısı

```text
dosya_encrypt/
├── hfzcrypt.c      # CLI argüman yönetimi ve parola girdi arayüzü
├── crypto.c        # OpenSSL EVP streaming şifreleme motoru
├── crypto.h        # Başlık yapıları ve fonksiyon tanımları
└── README.md       # Dokümantasyon
```

---

## Lisans

Bu proje Apache License 2.0 ile lisanslanmıştır.

**Geliştirici:** h4fizA  
**Sürüm:** 1.0
