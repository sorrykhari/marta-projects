import json
import requests

def load_config(path="config.json"):
    with open(path, "r") as f:
        return json.load(f)

def call_api(api_url, api_key):
    headers = {"Authorization": f"Bearer {api_key}"}
    response = requests.get(api_url, headers=headers)
    response.raise_for_status()
    return response.json()

if __name__ == "__main__":
    config = load_config()

    api_url = config["rail_url"]
    api_key = config["rail_key"]

    try:
        data = call_api(api_url, api_key)
        for item in data:
            if item["DIRECTION"] == "E" and item["STATION"] == "INMAN PARK STATION":
              print(f"{item["DIRECTION"]}-BOUND {item["LINE"]} ARRIVING IN {item["WAITING_TIME"]}")
            elif item["DIRECTION"] == "W" and item["STATION"] == "INMAN PARK STATION":
              print(f"{item["DIRECTION"]}-BOUND {item["LINE"]} ARRIVING IN {item["WAITING_TIME"]}")
    except Exception as e:
        print("Error calling API:", e)
