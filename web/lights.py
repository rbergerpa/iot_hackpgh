#!/usr/bin/env python

from flask import Flask, request, json, jsonify
import paho.mqtt.client as mqtt
import time

PORT = 8080

MQTT_BROKER = '127.0.0.1'

app = Flask(__name__, static_url_path='')

mqtt_client = mqtt.Client()
mqtt_client.connect(MQTT_BROKER)
mqtt_client.loop_start()

def start():
    app.run(host='0.0.0.0', port=PORT, debug=True)

@app.route('/')
def root():
    return app.send_static_file('index.html')

@app.route('/publish', methods=['PUT'])
def publish_message():
    json = request.get_json()
    topic = json['topic']
    message = json['message']
    
    print 'topic', topic
    print 'message', message

    mqtt_client.publish(topic, message)

    return "Ok"

if __name__ == "__main__":
    start()
