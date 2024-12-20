# TSL2591MI

Yet Another Arduino ams TSL2591 lux sensor library

## Features:
- Supports I2C via the Wire library 
- Supports other interfaces (e.g. software I2C) via inheritance
- Supports interrupts generated by the sensor
- Can be set up for specific light spectra
- Never blocks or delays (except for convenience functions)

more info in the [wiki](https://bitbucket.org/christandlg/tsl2591mi/wiki/) 


## Changelog:

- 0.10.0
    - merged !1 - many thanks to https://github.com/janscience for their contribution
        - fixes INTEGRATION_TIME defines
        - partially fixes #2
        - alternate initialization mode supported for TSL2591TwoWire class to match 3rd party libraries
        - code cleanup         

- 0.9.1 
    - fixed incorrect calculation of brightness threshold value in setALSInterruptThresholdBrightness()

- 0.9.0 
    - initial commit
