import paho.mqtt.client as mqtt
import json

# MQTT connection parameters
BROKER_IP = "192.168.230.1"
PORT = 1883
TOPIC = "application/3/device/70b3d57ed004c7de/rx"
OUTPUT_FILE = "img_data.txt"

# Callback: on connect
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Connected to MQTT Broker")
        client.subscribe(TOPIC)
    else:
        print(f"❌ Failed to connect, return code {rc}")

# Callback: on message
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode('utf-8'))
        img_data = payload.get("object", {}).get("img", "")
        if img_data:
            with open(OUTPUT_FILE, "a") as f:
                f.write(img_data)  # append without newline or space
            print("📝 img data appended")
    except Exception as e:
        print(f"⚠️ Error parsing message: {e}")

# Setup MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER_IP, PORT, keepalive=60)
client.loop_forever()
