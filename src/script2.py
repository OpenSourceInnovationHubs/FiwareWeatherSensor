import paho.mqtt.client as mqtt
import time
import ssl
import random
import string
import requests
import json
from functions import process_message, post_or_update_entity, create_subscription

subscription_exists = False

# Subscriber
def on_connect_subscriber(client, userdata, flags, reason_code, properties):
    topic = "/fhtw/368415464/rohatsch/we2j332"
    client.subscribe(topic, 0)
    print(f"{reason_code}! We are connected to topic: {topic}")

def on_message_subscriber(client, userdata, message):
    message = json.loads(message.payload.decode("utf-8"))
    ngsi_entity = process_message(message)
    post_or_update_entity(ngsi_entity)


def main():
    response = requests.get(f"http://localhost:1026/ngsi-ld/v1/subscriptions/urn:ngsi-ld:Subscription:1")
    if response.status_code == 404:
        create_subscription()
    else:
        print("Subscription already exists.")

    client_subscriber = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client_subscriber.on_connect = on_connect_subscriber
    client_subscriber.on_message = on_message_subscriber

    broker = "test.mosquitto.org"

    print("Connecting to broker for subscribing...")
    client_subscriber.tls_set(cert_reqs=ssl.CERT_NONE)
    client_subscriber.tls_insecure_set(True)
    client_subscriber.connect(broker, 8883, 60)
    client_subscriber.loop_start()
    print("Connected for subscribing - loop started")

    # uncomment to keep the script running
    while True:
        pass

    #uncomment to stop after 60 seconds
    # time.sleep(60)
    # # Stopping subscriber loop
    # client_subscriber.loop_stop()
    # client_subscriber.disconnect()
    # print("Disconnected for subscribing.")


if __name__ == "__main__":
    main()
