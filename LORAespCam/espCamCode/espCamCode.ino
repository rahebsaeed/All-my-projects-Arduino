// ======================================================================
// FINAL CODE: ESP32-CAM Image Capture & LoRaWAN Chunk Sender
// Inspired by the working simple JSON sender.
// ======================================================================
// Core libraries
#include <ArduinoJson.h>
#include "RAK811.h"
#include "esp_camera.h"
#include "mbedtls/base64.h"
// --- Use the standard AI Thinker camera model ---
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
// --- Serial port for monitoring on PC (from your working code) ---
#define DebugSerial Serial
// --- Serial port for RAK811 (from your working code) ---
#define RAK811_RX 15  // ESP32's RX, connects to RAK811's TX
#define RAK811_TX 14  // ESP32's TX, connects to RAK811's RX
HardwareSerial loraSerial(1); // Use UART 1 of the ESP32
// Initialize RAK811 library
RAK811 RAKLoRa(loraSerial, DebugSerial);
// --- Payload & Chunking Configuration ---
#define MAX_JSON_PAYLOAD_SIZE 120
#define JSON_WRAPPER_SIZE 7        // Characters for {"im":""}
#define JSON_ESCAPE_MARGIN 10      // Safety buffer for escaped characters like '/'
#define BASE64_DATA_CHUNK_SIZE (MAX_JSON_PAYLOAD_SIZE - JSON_WRAPPER_SIZE - JSON_ESCAPE_MARGIN)
// --- Image Settings ---
#define IMAGE_QUALITY 12
#define FRAME_SIZE FRAMESIZE_QVGA  // 320x240 is a good balance of size and detail
// --- Global State Variables ---
String base64Image;
unsigned int currentChunkIndex = 0;
bool imageCapturedAndReady = false;
// Function Prototypes
void captureAndEncodeImage();
void sendNextChunk();
void setup() {
// Start the serial monitor connection
DebugSerial.begin(115200);
while (!DebugSerial);
delay(2000); // Give time for monitor to connect
DebugSerial.println("\n--- ESP32-CAM Image to LoRaWAN Sender ---");
// Start the serial connection to the RAK811 module
loraSerial.begin(115200, SERIAL_8N1, RAK811_RX, RAK811_TX);
DebugSerial.println("RAK811 serial port initialized.");
// --- Initialize Camera ---
camera_config_t config;
config.ledc_channel = LEDC_CHANNEL_0;
config.ledc_timer = LEDC_TIMER_0;
config.pin_d0 = Y2_GPIO_NUM;
config.pin_d1 = Y3_GPIO_NUM;
config.pin_d2 = Y4_GPIO_NUM;
config.pin_d3 = Y5_GPIO_NUM;
config.pin_d4 = Y6_GPIO_NUM;
config.pin_d5 = Y7_GPIO_NUM;
config.pin_d6 = Y8_GPIO_NUM;
config.pin_d7 = Y9_GPIO_NUM;
config.pin_xclk = XCLK_GPIO_NUM;
config.pin_pclk = PCLK_GPIO_NUM;
config.pin_vsync = VSYNC_GPIO_NUM;
config.pin_href = HREF_GPIO_NUM;
config.pin_sscb_sda = SIOD_GPIO_NUM;
config.pin_sscb_scl = SIOC_GPIO_NUM;
config.pin_pwdn = PWDN_GPIO_NUM;
config.pin_reset = RESET_GPIO_NUM;
config.xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_JPEG;
config.frame_size = FRAME_SIZE;
config.jpeg_quality = IMAGE_QUALITY;
config.fb_count = 1;
esp_err_t err = esp_camera_init(&config);
if (err != ESP_OK) {
DebugSerial.printf("Camera init failed with error 0x%x\n", err);
DebugSerial.println("Check camera model, pins, and especially your 5V power supply!");
while(1); // Halt on failure
}
DebugSerial.println("Camera initialized successfully.");
DebugSerial.println("It's taking a photo install Camera");

delay(10000); // Wait
// --- Capture and Encode the Image ONCE ---
captureAndEncodeImage();
DebugSerial.println("Setup complete. Starting to send image chunks...");
}
void loop() {
// Check if we have an image ready and if there are more chunks to send
if (imageCapturedAndReady && (currentChunkIndex * BASE64_DATA_CHUNK_SIZE < base64Image.length())) {
sendNextChunk();
// Wait before sending the next chunk to respect LoRaWAN duty cycles.
// Adjust this delay as needed for your network (e.g., 5-10 seconds).
delay(5000);
} else if (imageCapturedAndReady) {
DebugSerial.println("\n--- All image chunks have been sent. ---");
imageCapturedAndReady = false; // Stop sending to prevent re-sending
}
}
void captureAndEncodeImage() {
DebugSerial.println("Capturing image...");
camera_fb_t *fb = esp_camera_fb_get();
if (!fb) {
DebugSerial.println("!!! Camera capture failed!");
return;
}
DebugSerial.printf("Image captured. Size: %d bytes\n", fb->len);
size_t encoded_len = 0;
mbedtls_base64_encode(NULL, 0, &encoded_len, fb->buf, fb->len);
char *encoded_buffer = (char *)malloc(encoded_len + 1);
if (!encoded_buffer) {
DebugSerial.println("!!! Failed to allocate memory for Base64 string.");
esp_camera_fb_return(fb);
return;
}
if (mbedtls_base64_encode((unsigned char *)encoded_buffer, encoded_len, &encoded_len, fb->buf, fb->len) != 0) {
DebugSerial.println("!!! Base64 encoding failed.");
free(encoded_buffer);
esp_camera_fb_return(fb);
return;
}
encoded_buffer[encoded_len] = '\0';
base64Image = String(encoded_buffer);
free(encoded_buffer);
esp_camera_fb_return(fb); // IMPORTANT: Free the frame buffer
DebugSerial.printf("Base64 encoding complete. Total size: %d bytes\n", base64Image.length());
int totalChunks = (base64Image.length() + BASE64_DATA_CHUNK_SIZE - 1) / BASE64_DATA_CHUNK_SIZE;
DebugSerial.printf("Image will be sent in %d chunks.\n\n", totalChunks);
imageCapturedAndReady = true;
}
void sendNextChunk() {
// 1. Slice the Base64 string
int startPos = currentChunkIndex * BASE64_DATA_CHUNK_SIZE;
String chunkData = base64Image.substring(startPos, startPos + BASE64_DATA_CHUNK_SIZE);
// 2. Create the JSON payload: {"im":"...data..."}
String jsonPayload = "{"im":"" + chunkData + ""}";
DebugSerial.println("JSON Payload: " + jsonPayload);
// Safety check
if (jsonPayload.length() > MAX_JSON_PAYLOAD_SIZE) {
DebugSerial.printf("!!! ERROR: Payload is too large! Size: %d. Skipping chunk.\n", jsonPayload.length());
currentChunkIndex++;
return;
}
// 3. Convert JSON to Hex
String hexPayload;
hexPayload.reserve(jsonPayload.length() * 2);
for (char c : jsonPayload) {
if (c < 0x10) hexPayload += "0";
hexPayload += String(c, HEX);
}
// 4. Build and send the AT command
String command = "at+send=lora:3:" + hexPayload;
int totalChunks = (base64Image.length() + BASE64_DATA_CHUNK_SIZE - 1) / BASE64_DATA_CHUNK_SIZE;
DebugSerial.printf("--- Sending Chunk %d / %d ---\n", currentChunkIndex + 1, totalChunks);
DebugSerial.printf("JSON Payload Size: %d bytes\n", jsonPayload.length());
if (RAKLoRa.sendRawCommand(command)) {
DebugSerial.println(">>> Command sent successfully to RAK811.");
currentChunkIndex++;
} else {
DebugSerial.println(">>> FAILED to send command. Will retry on next loop.");
}
}