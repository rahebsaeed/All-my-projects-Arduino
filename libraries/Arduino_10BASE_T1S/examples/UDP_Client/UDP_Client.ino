/*
 * This example has been tested with the Arduino 10BASE-T1S (T1TOS) shield.
 *
 * Author:
 *  Alexander Entinger
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_10BASE_T1S.h>

#include <SPI.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint8_t const T1S_PLCA_NODE_ID = 1;

static IPAddress const ip_addr     {192, 168,  42, 100 + T1S_PLCA_NODE_ID};
static IPAddress const network_mask{255, 255, 255,   0};
static IPAddress const gateway     {192, 168,  42, 100};

static T1SPlcaSettings const t1s_plca_settings{T1S_PLCA_NODE_ID};
static T1SMacSettings const t1s_default_mac_settings;

static IPAddress const UDP_SERVER_IP_ADDR = {192, 168,  42, 100 + 0};
static uint16_t const UDP_CLIENT_PORT = 8889;
static uint16_t const UDP_SERVER_PORT = 8888;

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

auto const tc6_io = new TC6::TC6_Io(
#if defined(ARDUINO_GIGA) || defined(ARDUINO_PORTENTA_C33)
  SPI1
#else
  SPI
#endif
  , CS_PIN
  , RESET_PIN
  , IRQ_PIN);
auto const tc6_inst = new TC6::TC6_Arduino_10BASE_T1S(tc6_io);
Arduino_10BASE_T1S_UDP udp_client;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  /* Initialize digital IO interface for interfacing
   * with the LAN8651.
   */
  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN),
                  []() { tc6_io->onInterrupt(); },
                  FALLING);

  /* Initialize IO module. */
  if (!tc6_io->begin())
  {
    Serial.println("'TC6_Io::begin(...)' failed.");
    for (;;) { }
  }

  MacAddress const mac_addr = MacAddress::create_from_uid();

  if (!tc6_inst->begin(ip_addr
    , network_mask
    , gateway
    , mac_addr
    , t1s_plca_settings
    , t1s_default_mac_settings))
  {
    Serial.println("'TC6::begin(...)' failed.");
    for (;;) { }
  }

  Serial.print("IP\t");
  Serial.println(ip_addr);
  Serial.println(mac_addr);
  Serial.println(t1s_plca_settings);
  Serial.println(t1s_default_mac_settings);

  if (!udp_client.begin(UDP_CLIENT_PORT))
  {
    Serial.println("begin(...) failed for UDP client");
    for (;;) { }
  }

  Serial.println("UDP_Client");
}

void loop()
{
  /* Services the hardware and the protocol stack.
   * Must be called cyclic. The faster the better.
   */
  tc6_inst->service();

  static unsigned long prev_beacon_check = 0;
  static unsigned long prev_udp_packet_sent = 0;

  auto const now = millis();

  if ((now - prev_beacon_check) > 1000)
  {
    prev_beacon_check = now;
    if (!tc6_inst->getPlcaStatus(OnPlcaStatus))
      Serial.println("getPlcaStatus(...) failed");
  }

  if ((now - prev_udp_packet_sent) > 1000)
  {
    static int tx_packet_cnt = 0;

    prev_udp_packet_sent = now;

    /* Prepare UDP packet. */
    uint8_t udp_tx_msg_buf[256] = {0};
    int const tx_packet_size = snprintf((char *)udp_tx_msg_buf, sizeof(udp_tx_msg_buf), "Single-Pair Ethernet / 10BASE-T1S: packet cnt = %d", tx_packet_cnt);

    /* Send a UDP packet to the UDP server. */
    udp_client.beginPacket(UDP_SERVER_IP_ADDR, UDP_SERVER_PORT);
    udp_client.write(udp_tx_msg_buf, tx_packet_size);
    udp_client.endPacket();

    Serial.print("[");
    Serial.print(millis());
    Serial.print("] UDP_Client sending: \"");
    Serial.print(reinterpret_cast<char *>(udp_tx_msg_buf));
    Serial.println("\"");

    tx_packet_cnt++;
  }

  /* Check for incoming UDP packets. */
  int const rx_packet_size = udp_client.parsePacket();
  if (rx_packet_size)
  {
    /* Print some metadata from received UDP packet. */
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] Received ");
    Serial.print(rx_packet_size);
    Serial.print(" bytes from ");
    Serial.print(udp_client.remoteIP());
    Serial.print(" port ");
    Serial.print(udp_client.remotePort());
    Serial.print(", data = \"");

    /* Read from received UDP packet. */
    size_t const UDP_RX_MSG_BUF_SIZE = 16 + 1; /* Reserve the last byte for the '\0' termination. */
    uint8_t udp_rx_msg_buf[UDP_RX_MSG_BUF_SIZE] = {0};
    int bytes_read = udp_client.read(udp_rx_msg_buf, UDP_RX_MSG_BUF_SIZE - 1);
    while(bytes_read != 0)
    {
      /* Print received data to Serial. */
      udp_rx_msg_buf[bytes_read] = '\0'; /* Terminate buffer so that we can print it as a C-string. */
      Serial.print(reinterpret_cast<char *>(udp_rx_msg_buf));

      /* Continue reading. */
      bytes_read = udp_client.read(udp_rx_msg_buf, UDP_RX_MSG_BUF_SIZE - 1);
    }
    Serial.println("\"");

    /* Finish reading the current packet. */
    udp_client.flush();
  }
}

static void OnPlcaStatus(bool success, bool plcaStatus)
{
  if (!success)
  {
    Serial.println("PLCA status register read failed");
    return;
  }

  if (plcaStatus)
    Serial.println("PLCA Mode active");
  else {
    Serial.println("CSMA/CD fallback");
    tc6_inst->enablePlca();
  }
}
