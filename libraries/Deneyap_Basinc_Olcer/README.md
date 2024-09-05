# Deneyap Barometer Arduino Library
[FOR TURKISH VERSION](docs/README_tr.md) ![trflag](https://github.com/deneyapkart/deneyapkart-arduino-core/blob/master/docs/tr.png)

***** Add photo ****

Arduino library for Deneyap Barometer MS5637-02BA03

## :mag_right:Specifications 
- `Product ID` **M40**, **mpv1.0**
- `MCU` MS5637-02BA03
- `Weight` 
- `Module Dimension` 25,4 mm x 25,4 mm
- `I2C address` 0x76

| Address |	| 
| :---    |:---|
| 0x76 | default address |

## :closed_book:Documentation
[Deneyap Barometer](https://docs.deneyapkart.org/en/content/contentDetail/deneyap-module-deneyap-barometric-pressure-m40)

[Deneyap Barometer Shematic](https://cdn.deneyapkart.org/media/upload/userFormUpload/rlIbYuEQpY6D71aVr0apsTyrQbVXPrZY.pdf)

[Deneyap Barometer Mechanical Drawing](https://cdn.deneyapkart.org/media/upload/userFormUpload/NjhCgZqNpQx17C8GAnLewFxnY0KyZKCy.pdf)

[MS5637-02BA03_datasheet](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5637-02BA03&DocType=Data+Sheet&DocLang=English)

[How to install a Arduino Library](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries)

## :pushpin:Deneyap Barometer
This Arduino library allows you to use Deneyap Barometer MS5637-02BA03 with I2C peripheral. You can use this library in your projects with any Arduino compatible board with I2C peripheral.

## :globe_with_meridians:Repository Contents
- `/docs ` README_tr.md and product photos
- `/examples ` Examples with .ino extension sketches
- `/src ` Source files (.cpp .h)
- `keywords.txt ` Keywords from this library that will be highlighted in the Arduino IDE
- `library.properties ` General library properties for the Arduino package manager

## Version History
1.0.1 - update library and texts info 

1.0.0 - initial release

## :rocket:Hardware Connections
- Deneyap Barometer and Board can be connected with I2C cable
- or 3V3, GND, SDA and SCL pins can be connected with jumper cables

|Barometer| Function | Board pins | 
|:--- |   :---  | :---|
|3.3V | Power   |3.3V |      
|GND  | Ground  | GND | 
|SDA  | I2C Data  | SDA pin |
|SCL  | I2C Clock | SCL pin |
| NC| no connection|         |

## :bookmark_tabs:License Information
Please review the [LICENSE](https://github.com/deneyapkart/deneyap-basinc-olcer-arduino-library/blob/master/LICENSE) file for license information.