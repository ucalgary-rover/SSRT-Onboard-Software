import argparse
import paho.mqtt.client as mqtt

# Callback function when a message is received
def on_message(client, userdata, msg):
    print(f"Message received on topic {msg.topic}: {msg.payload.decode()}")

def main():
    # Create an argument parser
    parser = argparse.ArgumentParser(description="Test MQTT Subscriber")
    
    # Define arguments for broker, port, and topic
    parser.add_argument('-b', '--broker', type=str, required=True, help="MQTT broker address (hostname or IP)")
    parser.add_argument('-p', '--port', type=int, default=1883, help="Port to connect to the broker (default is 1883)")
    parser.add_argument('-t', '--topic', type=str, required=True, help="Topic to subscribe to")

    # Parse the command-line arguments
    args = parser.parse_args()

    # Connect to the broker
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(args.broker, args.port)

    # Subscribe to the specified topic
    client.subscribe(args.topic)
    
    # Start the loop to keep the script running and listening for messages
    print(f"Subscribed to topic '{args.topic}' on {args.broker}:{args.port}")
    client.loop_forever()

if __name__ == "__main__":
    main()


# Usage
# python mqtt_subscriber.py -b [ipaddress] -p [port] -t [topic]