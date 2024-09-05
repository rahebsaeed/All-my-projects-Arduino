/*
 *   BasincSicaklikHataOkuma,
 *   Bu örnekte temel konfigürasyon ayarları yapılmaktadır.
 *   Sensör haberleşmesinde hata bulunursa seri terminale yazdırmaktadır.
 *   Sensörden basınç ve sıcaklık verilerimi okumaktadır. Saniyede bir bu verileri seri port ekranına yazdırmaktadır.
 *
 *   Bu algılayıcı I2C haberleşme protokolü ile çalışmaktadır.
 *
 *   Bu örnek Deneyap Basınç Ölçer için oluşturulmuştur
 *      ------> https://docs.deneyapkart.org/tr/content/contentDetail/deneyap-module-deneyap-barometric-pressure-m40 <------
 *      ------> https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library <------
*/
#include <Deneyap_BasincOlcer.h>                    // Deneyap Basınç Ölçer kütüphanesi eklenmmesi

AtmosphericPressure BaroSensor;                     // AtmosphericPressure için class tanımlanması

float basinc;
float sicaklik;

void setup() {
    Serial.begin(115200);                           // Seri haberleşme başlatılması
    BaroSensor.begin(0x76);                         // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
}

void loop() {
    if (!BaroSensor.isOK()) {
        Serial.print("Sensor bulunamadi. Hata: ");
        Serial.println(BaroSensor.getError());      // Hata seri port ekranına yazdırılması
        BaroSensor.begin(0x76);                     // Cihazların haberleşmesi yeniden başlatılması
    } else {
        basinc = BaroSensor.getPressure();          // Basınç değeri okunması
        Serial.print("Basinc: ");
        Serial.println(basinc);                     // Basınç değerinin seri port ekranına yazdırılması

        sicaklik = BaroSensor.getTemp();            // Sıcaklık değeri okundu
        Serial.print("Sicaklik: ");
        Serial.println(sicaklik);                   // Sıcaklık değerinin seri port ekranına yazdırılması
    }
    delay(1000);                                    // 1 saniye bekleme süresi
}
