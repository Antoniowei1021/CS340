import requests
from flask import Flask, request, jsonify, render_template, send_file
from PIL import Image
import random
app = Flask(__name__)

cache_data = None
resp = None
image_list = ["cme.png", "fender_customshop.png", "Gibson.png", "f.png", "g.png"]
info = {
    "author" : "Eric Wei",
    "url": "http://fa23-cs340-025.cs.illinois.edu:34000/",
    "token" : "ab922882-1605-431f-8326-893b0a93e320"
}

vote_info = {
  "voteToken" : "8ac3783a-b00b-48ca-9f4b-1253f4983aff",
  "xloc" : cache_data["xloc"],
  "yloc" : cache_data["yloc"]
}

url1 = 'http://fa23-cs340-adm.cs.illinois.edu:5000/registerClient/chiwei2'
url2 = 'http://fa23-cs340-adm.cs.illinois.edu:5000/vote/chiwei2'
result = requests.put(url1, json=info).json()
result2 = requests.put(url2, json=vote_info).json()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/uploadImage', methods=["POST"])
def POST_upload_Image():
    global image_list, result
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
            vote_tok = cache_data["voteToken"]
            return "Authorization token invalid", 455
        else:
            return "Success", 200
    return "Not approved", 500
    
@app.route('/image', methods=["GET"])
def get_image():
    global cache_data
    # Check if an image has been approved
    if cache_data and cache_data["approved"] == True:
        return send_file("resized_image.png", mimetype='image/png')
    else:
         # if no image is approved
        return "Approved image not found", 404

@app.route('/tile', methods=["GET"])
def get_tile():
    global cache_data, result
    if cache_data and cache_data.get("approved"):
        try:
            img = Image.open("resized_image.png")
            # Calculate the coordinates for cropping
            x_loc = cache_data["xloc"]
            y_loc = cache_data["yloc"]
            x_coord1 = x_loc * result['tilesize']
            y_coord1 = y_loc * result['tilesize']
            x_coord2 = x_coord1 + result['tilesize']
            y_coord2 = y_coord1 + result['tilesize']
            # Crop the image
            im1 = img.crop((x_coord1, y_coord1, x_coord2, y_coord2))
            im1.save('myimage_cropped.png')
            return send_file("myimage_cropped.png", mimetype='image/png')
        except Exception as e:
            return f"Error processing tile: {e}", 500
    else:
        # If no section is found
        return "Section not found", 404

@app.route('/votes', methods=["GET"])
def get_votes():
    global resp
    resp = request.get_json()
    return jsonify({"votes" : resp["votes"]}), 200

@app.route('/votes', methods=["PUT"])
def put_votes():
    global resp, info
    data = request.get_json()
    # Check if the token match
    if data["token"] != info["token"]:
        return "Unauthorized", 401
    # Check if the seq is greater than the last seq
    if resp and "seq" in resp and data["seq"] <= resp["seq"]:
        return "Conflict", 409
    # Update stored votes and seq
    resp = {
        "votes": data["votes"],
        "seq": data["seq"]
    }
    return "Votes updated", 200