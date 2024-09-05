# Deneyap Basınç Ölçer Arduino Kütüphanesi

***** Fotoğraf eklenecek ****

Deneyap Basınç Ölçer MS5637-02BA03 için Arduino kütüphanesidir.

## :mag_right:Özellikler 
- `Ürün ID` **M40**, **mpv1.0**
- `MCU` MS5637-02BA03
- `Ağırlık`
- `Modul Boyutları` 25,4 mm x 25,4 mm
- `I2C Adres` 0x76

| Adres |  | 
| :--- | :---     |
| 0x76 | varsayılan adres |

## :closed_book:Dokümanlar
[Deneyap Basınç Ölçer](https://docs.deneyapkart.org/tr/content/contentDetail/deneyap-module-deneyap-barometric-pressure-m40)

[Deneyap Basınç Ölçer Şematik](https://cdn.deneyapkart.org/media/upload/userFormUpload/rlIbYuEQpY6D71aVr0apsTyrQbVXPrZY.pdf)

[Deneyap Basınç Ölçer Teknik Çizim](https://cdn.deneyapkart.org/media/upload/userFormUpload/NjhCgZqNpQx17C8GAnLewFxnY0KyZKCy.pdf)

[MS5637-02BA03_datasheet](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5637-02BA03&DocType=Data+Sheet&DocLang=English)

[Arduino IDE'de Kütüphane Nasıl İndirilir](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries)

## :pushpin:Deneyap Basınç Ölçer
Bu Arduino kütüphanesi Deneyap Basınç Ölçer MS5637-02BA03 I2C çevre birimi ile kullanılmasını sağlar. Arduino ile uyumlu, I2C çevre birimine sahip herhangi bir geliştirme kartı ile bu kütüphaneyi projelerinizde kullanabilirsiniz.

## :globe_with_meridians:Repo İçeriği
- `/docs` README_tr.md ve ürün fotoğrafları
- `/examples` .ino uzantılı örnek uygulamalar
- `/src` kütüphane için .cpp ve .h uzantılı dosyalar
- `keywords.txt` Arduino IDE'de vurgulanacak anahtar kelimeler
- `library.properties` Arduino yöneticisi için genel kütüphane özellikleri

## Sürüm Geçmişi
1.0.1 - kütüphane ve metin bilgileri güncellendi

1.0.0 - ilk sürüm

## :rocket:Donanım Bağlantıları
- Deneyap Basınç Ölçer ile kullanılan geliştirme kartı I2C kablosu ile bağlanabilir
- veya jumper kablolar ile 3V3, GND, SDA ve SCL bağlantıları yapılabilir. 

| Basınç Ölçer | Fonksiyon| Kart pinleri |
| :---     | :---   |   :---  |
| 3.3V     |3.3V Besleme Gerilimi| 3.3V    |
| GND      |Toprak |GND      |
| SDA      |Çift Yönlü Veri Hattı| SDA pini |
| SCL      |Veri Hattı Zaman Senkronizasyon İşareti| SCL pini|
| NC       |bağlantı yok |   |

## :bookmark_tabs:Lisans Bilgisi 
Lisans bilgileri için [LICENSE](https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library/blob/master/LICENSE) dosyasını inceleyin.