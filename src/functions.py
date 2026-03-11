import datetime
import requests

ORION_URL = "http://localhost:1026/ngsi-ld/v1/entities"
NGSI_LD_CONTEXT = "https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context.jsonld"

def process_message(message):
    print(message);
    print("Received message:", str(message["temp"]) + "°C" + " ")

    ngsi_entity = {
        "id": message["id"],
        "type": "SensorReading",
        "@context": NGSI_LD_CONTEXT,
        "temperature": {
            "type": "Property",
            "value": message["temp"],
        },
        "humidity": {
            "type": "Property",
            "value": message["hum"],
        },
        "pressure": {
            "type": "Property",
            "value": message["pressure"],
        },
    }
    return ngsi_entity


def post_or_update_entity(entity):
    headers = {
        "Content-Type": "application/ld+json",
    }

    response = requests.patch(f"{ORION_URL}/{entity['id']}/attrs", headers=headers, json=entity)
    if response.status_code in [200, 204]:
        print(f"Entity {entity['id']} updated:", response.status_code, response.text)

    if response.status_code == 404:
        response = requests.post(ORION_URL, headers=headers, json=entity)
        print(f"Entity {entity['id']} posted:", response.status_code, response.text)        
    
    return response.status_code


def create_subscription():
    headers = {
        "Content-Type": "application/ld+json",
    }
    subscription = {
        "id": "urn:ngsi-ld:Subscription:1",
        "description": "Notify QuantumLeap",
        "type": "Subscription",
        "entities": [
            {
                "type": "SensorReading"
            }
        ],
        "watchedAttributes": ["temperature"],
        "notification": {
            "attributes": ["temperature", "humidity", "pressure"],
            "endpoint": {
                "uri": "http://quantumleap:8668/v2/notify",
                "accept": "application/json",
            }
        },
        "@context": NGSI_LD_CONTEXT,        
    }

    response = requests.post("http://localhost:1026/ngsi-ld/v1/subscriptions", headers=headers, json=subscription)
    if response.status_code == 201:
        print("Subscription created successfully.")
    else:
        print("Failed to create subscription:", response.status_code, response.text)
