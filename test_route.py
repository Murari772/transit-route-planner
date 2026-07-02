import requests
import json
res = requests.get('http://localhost:8080/route?city=Delhi&source=Rajiv+Chowk&destination=Anand+Vihar&criterion=least_time').json()
print(json.dumps(res, indent=2))
