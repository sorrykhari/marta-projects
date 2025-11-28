from flask import Flask, jsonify
import requests
import json

app = Flask(__name__)

def load_config(path="config.json"):
    with open(path, "r") as f:
        return json.load(f)

def call_api(api_url, api_key):
    headers = {"Authorization": f"Bearer {api_key}"}
    response = requests.get(api_url, headers=headers)
    response.raise_for_status()
    return response.json()

@app.route('/')
def home_page():
  return "Server is up."

@app.route('/train-times')
def train_page():
  config = load_config()
  train_list = []

  api_url = config["rail_url"]
  api_key = config["rail_key"]

  try:
     data = call_api(api_url, api_key)
     for item in data:
        if item["DIRECTION"] == "E" and item["STATION"] == "INMAN PARK STATION":
            train_list.append(item)
        elif item["DIRECTION"] == "W" and item["STATION"] == "INMAN PARK STATION":
            train_list.append(item)
     return jsonify(train_list)
  except Exception as e:
     return f"There was an error lol {e}"

if __name__ == "__main__":
  app.run(host='0.0.0.0', port=5000, debug=True)