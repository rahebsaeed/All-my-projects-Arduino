# LoRa Module RAK811 with Arduino as Receiver
# LoRa Module RAK811 with Arduino as Receiver

This project demonstrates how to use the RAK811 module in **LoRa P2P** mode to receive data. The code configures the RAK811 module, extracts hexadecimal data from the received payload, converts it back to ASCII text, and displays the decoded information.

---

## Features

- Receives data via LoRa P2P using the RAK811 module.
- Extracts valid hexadecimal data from the raw payload.
- Converts hexadecimal data into readable ASCII text.

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
   - Open the Arduino IDE and upload the provided receiver code to your Arduino board.

---

## Configuration

The code initializes the RAK811 module and sets it to **LoRa P2P** mode. Below are the configurable parameters:

- **Frequency**: `869525000` Hz  
- **Spreading Factor**: `12`  
- **Bandwidth**: `0` (125 kHz)  
- **Coding Rate**: `1` (4/5)  
- **Preamble Length**: `8`  
- **Transmission Power**: `20` dBm  

You will find all commands for configuring the RAK811 in this text file: [Configuration LoRa RAK811](https://github.com/rahebsaeed/All-my-projects-Arduino/blob/main/configuration%20loard%20rak811.txt).  
You also need to download the RAK Serial Port Tool to configure the RAK811 module: [RAK_SERIAL_PORT_TOOL](https://downloads.rakwireless.com/LoRa/RAK811/Tools/RAK_SERIAL_PORT_TOOL_V1.2.1.zip).

---

# Related 
[LoRa Module RAK811 with Arduino as Sender](https://github.com/rahebsaeed/All-my-projects-Arduino/tree/main/LoRa_sender)
