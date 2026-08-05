# 🛡️ HFZCrypt v1.0 — High-Performance File Encryption Tool

**HFZCrypt**, saf **C** dilinde yazılmış, **OpenSSL 3.0+ EVP API** altyapısını kullanan komut satırı tabanlı yüksek başarımlı ve güvenli bir dosya şifreleme aracıdır. 

Hem **AES-256-GCM** hem de **ChaCha20-Poly1305** AEAD (Authenticated Encryption with Associated Data) algoritmalarını destekler. Büyük dosyaları 1 MB'lık parçalı akış (streaming buffer) yöntemiyle belleği şişirmeden ultra hızlı bir şekilde şifreler ve çözer.

---

## ✨ Öne Çıkan Özellikler

- **Çift AEAD Algoritması:** Donanım ivmeli **AES-256-GCM** ve yazılımsal optimize **ChaCha20-Poly1305** şifreleme desteği.
- **Yüksek Güvenlikli KDF:** Parolalardan 600.000 iterasyonlu `PBKDF2-HMAC-SHA256` ile 256-bit kriptografik anahtar türetimi.
- **CSRNG Rastgelelik:** `OpenSSL RAND_bytes` ile üretilen 16-byte Salt ve 12-byte IV (Nonce).
- **Veri Bütünlüğü ve Doğrulama:** GCM / Poly1305 **Authentication Tag** (16-byte) doğrulaması. Yanlış parola veya kurcalanmış veride dosya derhal reddedilir.
- **Akış Yapısı (Streaming I/O):** 1 MB sabit bellek tamponu sayesinde gigabaytlarca büyüklükteki dosyaları minimum RAM kullanımıyla şifreleme.
- **Sihirli Başlık (Magic Header):** `"hfz1638!"` imzası ile format doğrulaması.
- **Gizli Parola Girdisi:** `termios` kütüphanesi ile terminalde yazılan parolanın gizlenmesi (`~ECHO`).
- **POSIX CLI:** `getopt_long` ile esnek komut satırı parametreleri (`-e`, `-d`, `-a`).

---

## ⚡ 1 GB Dosya Performans Benchmark Testi

Aşağıdaki test sonuçları **1 GB** büyüklüğündeki test dosyası üzerinde elde edilmiştir:

| Algoritma | Şifreleme (Encrypt) | Şifre Çözme (Decrypt) | Ortalama Hız |
|---|---|---|---|
| 🔒 **AES-256-GCM** | **~0.94 s** | **~1.01 s** | **~1.05 GB/s** |
| ⚡ **ChaCha20-Poly1305** | **~1.04 s** | **~1.28 s** | **~0.96 GB/s** |

> 🚀 **AES-NI** destekli modern işlemcilerde AES-256-GCM donanımsal ivme ile 1 GB veriyi **1 saniyenin altında** şifrelemektedir!

---

## 📦 `.hfz` Paket Başlık Formatı

Şifrelenmiş her dosyanın başına 53 baytlık sabit bir binary başlık (`Paket`) eklenir:

```text
+-------------------+----------------+---------------+---------------+---------------+----------------------+
| Magic Header (8B) | Algoritma (1B) | GCM/Tag (16B) |    IV (12B)   |   Salt (16B)  | Şifreli Veri (N B)   |
|   "hfz1638!"      | 1:AES / 2:CC20 | Auth Tag      |  Random Nonce | PBKDF2 Salt   | Encrypted Payload    |
+-------------------+----------------+---------------+---------------+---------------+----------------------+
```

---

## 🛠️ Derleme ve Kurulum

### Bağımlılıklar
Debian / Ubuntu / Parrot OS için OpenSSL geliştirme paketleri:

```bash
sudo apt update
sudo apt install build-essential libssl-dev
```

### Derleme (O3 Optimizasyonlu)

```bash
gcc -O3 hfzcrypt.c crypto.c -o hfzcrypt -lcrypto
```

---

## 🚀 Kullanım ve Örnekler

### Yardım Menüsü
```bash
./hfzcrypt -h
```

### 1. Dosya Şifreleme (`-e`)

**AES-256-GCM ile Şifreleme:**
```bash
./hfzcrypt -e belge.pdf -a aes
```
*Çıktı:* `belge.pdf.hfz` adında şifreli dosya oluşturulur.

**ChaCha20-Poly1305 ile Şifreleme:**
```bash
./hfzcrypt -e video.mp4 -a chacha
```
*Çıktı:* `video.mp4.hfz` adında şifreli dosya oluşturulur.

---

### 2. Dosya Şifresi Çözme (`-d`)

```bash
./hfzcrypt -d belge.pdf.hfz
```
*Çıktı:* Başlık okunur, algoritma otomatik tespit edilir ve orijinal `belge.pdf` dosyası geri elde edilir.

---

## 📁 Proje Dosya Yapısı

```text
dosya_encrypt/
├── hfzcrypt.c      # CLI parametre kontrolü & terminal parola arayüzü
├── crypto.c        # OpenSSL EVP AES-GCM / ChaCha20 streaming kripto motoru
├── crypto.h        # Başlık veri yapıları (Paket) ve fonksiyon prototipleri
└── README.md       # Proje dokümantasyonu
```

---

## 📄 Lisans

Bu proje **Apache License 2.0** altında lisanslanmıştır.

---

**Geliştirici:** h4fizA
**Versiyon:** v1.0
