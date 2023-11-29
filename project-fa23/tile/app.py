import requests
from flask import Flask, request, jsonify
from PIL import Image


cache_data = None

info = {
    "author" : "Eric Wei",
    "url": "http://127.0.0.1:34000/",
    "token" : "ab922882-1605-431f-8326-893b0a93e320"
}
url = 'http://127.0.0.1:5000/registerClient/chiwei2'
result = requests.put(url, json=info).json()
print(result)
 
app = Flask(__name__)

resized_img = Image.open('Gibson.jpg')
resized_img = resized_img.resize((result['xdim'] * result['tilesize'], result['ydim'] * result['tilesize']))
resized_img.save("resized_image.png")

base_url = "http://127.0.0.1:5000"
with open("resized_image.png", 'rb') as image_file:
    files = {'image': image_file}
    response = requests.post(f"{base_url}/registerImage/chiwei2", files=files)
    print(response.status_code)

@app.route('/registered', methods=["PUT"])
def PUT_registered():
    global cache_data
    data = request.get_json()
    cache_data = data
    if data["approved"] == 'true':
        if data["authToken"] != info["token"]:
            return "Authorization token invalid", 455
        else:
            return "Success", 200
    return "Not approved", 456
        

