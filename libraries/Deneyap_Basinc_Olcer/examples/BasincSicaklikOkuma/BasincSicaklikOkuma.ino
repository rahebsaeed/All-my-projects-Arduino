/*
 *   BasincSicaklikOkuma örneği,
 *   Bu örnekte temel konfigürasyon ayarları yapılmaktadır.
 *   Sesmörden basınç ve Sıcaklık verilerimi okumaktadır. Saniyede bir bu verileri seri port ekranına yazdırmaktadır.
 *
 *   Bu algılayıcı I2C haberleşme protokolü ile çalışmaktadır.
 *
 *   Bu örnek Deneyap Basınç Ölçer için oluşturulmuştur
 *      ------> https://docs.deneyapkart.org/tr/content/contentDetail/deneyap-module-deneyap-barometric-pressure-m40 <------
 *      ------> https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library <------
*/
#include <Deneyap_BasincOlcer.h>        // Deneyap Basınç Ölçer kütüphanesi eklenmmesi

AtmosphericPressure BaroSensor;         // AtmosphericPressure için class tanımlanması

float basinc;
float sicaklik;

void setup() {
    Serial.begin(115200);               // Seri haberleşme başlatılması
    BaroSensor.begin(0x76);             // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
}

void loop() {     
    basinc = BaroSensor.getPressure();  // Basınç değerinin okunması
    Serial.print("Basinc: ");
    Serial.println(basinc);             // Basınç değeri seri port ekranına yazdırılması

    sicaklik = BaroSensor.getTemp();    // Sıcaklık değeri okunması
    Serial.print("Sicaklik: ");
    Serial.println(sicaklik);           // Sıcaklık değeri seri terminale yazdırılması
    delay(1000);                        // 1 saniye bekleme süresi
}
