## LoRa Module RAK811 with Arduino as Sender
# RAK811 LoRa Sender Example

This project demonstrates how to use the RAK811 module in **LoRa P2P** mode to send data. The code configures the RAK811 module, converts ASCII text into its hexadecimal equivalent, and sends the converted data over LoRa.

## Features

- Sends ASCII data as a hexadecimal string using the RAK811 module.
- Implements a function to convert ASCII text to hexadecimal format.
- Configurable LoRa parameters like frequency, spreading factor, bandwidth, coding rate, preamble length, and transmission power.

---

## Requirements

### Hardware
- **RAK811 Module**: LoRa P2P capable module.
- Arduino-compatible microcontroller.
- Jumper wires and breadboard.

### Libraries
1. [RAK811](https://github.com/RAKWireless/WisNode-Arduino-Library):
   - Used for interfacing with the RAK811 module.
2. **SoftwareSerial**:
   - To handle serial communication with the module.

---

## Installation Instructions

1. **Install Arduino IDE**  
   Download and install the [Arduino IDE](https://www.arduino.cc/en/software).

2. **Add RAK811 Library**  
   Install the RAK811 library in the Arduino IDE:
   - Open Arduino IDE.
   - Go to **Tools > Manage Libraries**.
   - Search for `RAK811`.
   - Click **Install**.

3. **Connect RAK811**  
   Wire the RAK811 module to your Arduino:
   - **TX** on RAK811 → Pin 10 on Arduino
   - **RX** on RAK811 → Pin 11 on Arduino
   - **GND** on RAK811 → GND on Arduino
   - **VCC** on RAK811 → 3.3V or 5V on Arduino (depending on module specifications)

4. **Upload the Code**  
   - Open the Arduino IDE and upload the code to your Arduino board.

---

## Configuration

The code initializes the RAK811 module and sets it to **LoRa P2P** mode. Below are the configurable parameters:
  You will find all commands for configuration RAK811 in this file text [Configuration LoRa RAK811]([https://www.arduino.cc/en/software](https://github.com/rahebsaeed/All-my-projects-Arduino/blob/main/configuration%20loard%20rak811.txt).
- **Frequency**: `869525000` Hz  
- **Spreading Factor**: `12`  
- **Bandwidth**: `0` (125 kHz)  
- **Coding Rate**: `1` (4/5)  
- **Preamble Length**: `8`  
- **Transmission Power**: `20` dBm  

You can modify these parameters in the code under the `rk_initP2P` function.

---

## Code Explanation

### ASCII to Hex Conversion
The `asciiToHex` function converts a string of ASCII characters into a hexadecimal representation for transmission:

```
String asciiToHex(String ascii) {
    String hex = "";
    for (int i = 0; i < ascii.length(); i++) {
        char c = ascii[i];
        if (c < 16) {
            hex += "0";
        }
        hex += String(c, HEX);
    }
    hex.toUpperCase();
    return hex;
}
```
