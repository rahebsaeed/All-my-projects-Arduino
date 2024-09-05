# Changelog

## v5.2.6
- Updated core to v3.1.3

## 5.2.5
- Updated core to v3.1.2

## 5.2.4
- Update core to support MMOD

## v5.2.3
- Moved pinMode to Begin

## v5.2.2
- Fixed bug in Config return values

## v5.2.1
- Added fix in includes to support Arduino AVR

## v5.2.0
- Added CMake Teensy MMOD support

## v5.1.0
- Added default constructor and a config method for configuring the sensor

## v5.0.0
- Merging Bme280 and Bme280-arduino
- Pulling in CMake tooling from the mcu-support repo
- Pulling in BST provided source files and wrapping with C++

## v2.0.1
- Put guards around digitalWriteFast to support microcontrollers that might not have that available
- Switched std::size_t to int to support microcontrollers without STL access

## v2.0.0
- Updated to match our [BME-280](https://github.com/bolderflight/bme280) library for flight software
- Updated license to MIT

## v1.0.2
- Updated license to GPLV3.

## v1.0.1
- Updating library.properties name.

## v1.0.0
- Modified to work with Arduino 1.5 format and creating a baseline release now.
