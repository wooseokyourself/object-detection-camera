import requests, json
        
def post(url, data):
    response = requests.post(url, json=data)
    return response.status_code, response.text
