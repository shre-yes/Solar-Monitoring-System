import paho.mqtt.client as mqtt
import json

BROKER = "localhost"
TOPIC = "energy/data"

# ================= CONNECT =================
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Broker!")
    client.subscribe(TOPIC)

# ================= MESSAGE =================
def on_message(client, userdata, msg):

    print("\n==============================")
    print(" New Energy Data Received")
    print("==============================")

    payload = msg.payload.decode()

    data = json.loads(payload)

    print(f"PV Voltage       : {data['pv_voltage']} V")
    print(f"PV Current       : {data['pv_current']} A")

    print(f"Battery Voltage  : {data['battery_voltage']} V")
    print(f"Battery Current  : {data['battery_current']} A")

    print(f"Load Voltage     : {data['load_voltage']} V")
    print(f"Load Current     : {data['load_current']} A")

    print(f"Load Power       : {data['load_power']} W")
    print(f"Load PF          : {data['load_pf']}")
    print(f"Load Frequency   : {data['load_frequency']} Hz")

# ================= MQTT CLIENT =================
client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, 1883, 60)

print("Listening for Energy Data...")

client.loop_forever()
