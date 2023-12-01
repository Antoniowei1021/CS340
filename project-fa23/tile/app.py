import requests
from flask import Flask, request, jsonify, render_template
from PIL import Image
import random
app = Flask(__name__)

cache_data = None
image_list = ["cme.png", "fender_customshop.png", "Gibson.png", "f.png", "g.png"]
info = {
    "author" : "Eric Wei",
    "url": "http://fa23-cs340-025.cs.illinois.edu:34000/",
    "token" : "ab922882-1605-431f-8326-893b0a93e320"
}
url = 'http://fa23-cs340-adm.cs.illinois.edu:5000/registerClient/chiwei2'
result = requests.put(url, json=info).json()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/uploadImage', methods=["POST"])
def POST_upload_Image():
    global image_list
    i = random.randint(0,4)
    resized_img = Image.open(image_list[i])
    resized_img = resized_img.resize((result['xdim'] * result['tilesize'], result['ydim'] * result['tilesize']))
    resized_img.save("resized_image.png", format='PNG')
    base_url = "http://fa23-cs340-adm.cs.illinois.edu:5000"
    with open("resized_image.png", 'rb') as image_file:
        files = {'file': image_file}
        response = requests.post(f"{base_url}/registerImage/chiwei2", files=files)
    if response.status_code == 500:
        return "Invalid size", response.status_code
    elif response.status_code == 416:
        return "No clientID", response.status_code
    elif response.status_code == 200:
        return "Success", 200
    else:
        return "", response.status_code
    

@app.route('/registered', methods=["PUT"])
def PUT_registered():
    global cache_data
    cache_data = request.get_json()
    print(cache_data)
    if cache_data["approved"] == True:
        if cache_data["authToken"] != info["token"]:
            return "Authorization token invalid", 455
        else:
            return "Success", 200
    return "Not approved", 500
    

