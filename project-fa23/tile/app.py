import requests
from flask import Flask, request, jsonify, render_template, send_file, json
from PIL import Image
import io
app = Flask(__name__)
i = 0
max_nb = 0
max_url = None
cache_data = None
resp = None
image_list = ["cme.png", "fender_customshop.png", "Gibson.png", "f.png", "g.png"]
info = {
    "author" : "Eric Wei",
    "url": "http://fa23-cs340-025.cs.illinois.edu:34000/",
    "token" : "ab922882-1605-431f-8326-893b0a93e320"
}
url1 = 'http://fa23-cs340-adm.cs.illinois.edu:5000/registerClient/chiwei2'
result = requests.put(url1, json=info).json()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/uploadImage', methods=["POST"])
def POST_upload_Image():
    global result
    if 'file' not in request.files:
        return "No file part", 400
    file = request.files['file']
    if file :
        img = Image.open(file.stream)
        resized_img = img.resize((result['xdim'] * result['tilesize'], result['ydim'] * result['tilesize']))
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
    return "Invalid file type", 400

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
    else:
        return "Section not found", 404

@app.route('/votes', methods=["GET"])
def get_votes():
    return jsonify({"votes" : resp["votes"]}), 200

@app.route('/votes', methods=['PUT'])
def put_votes():
    global resp, info
    data = request.get_json()
    # Check if the token match
    if data["authToken"] != info["token"]:
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

@app.route('/voteForTile', methods=['POST'])
def vote_tile():
    base_url = "http://fa23-cs340-adm.cs.illinois.edu:5000"
    print(request.form)
    xloc = request.form.get('tileX')
    yloc = request.form.get('tileY')
    input =  {
    "voteToken": cache_data["voteToken"],
    "xloc": xloc,
    "yloc": yloc
    }
    resp = requests.put(f"{base_url}/vote/chiwei2", json=input)
    if resp.status_code == 200:
        return "done", resp.status_code
    return "Error", resp.status_code

@app.route('/update', methods=["PUT"])
def put_update():
    current_votes = resp.get("votes", 0) if resp else 0
    global max_nb 
    data = request.get_json()
    print(data)
    if not data or 'authToken' not in data or 'neighbors' not in data:
        return jsonify({"error": "Invalid request"}), 400
    # Check if the token match
    if data["authToken"] != info["token"]:
        return "Unauthorized", 401
    neighbors = data['neighbors']
    for nb in neighbors:
        # pass urls
        print(1)
        response = requests.get(f"{nb}/votes")
        nb_votes = response.json()["votes"]
        if max_nb < nb_votes and nb_votes > current_votes:
            max_nb = nb_votes
            max_url = nb
    if max_nb == 0:
        return "No update needed", 201
    new_responses = requests.get(f"{max_url}/image")
    with open("updated_image.png", "wb") as f:
        f.write(new_responses.content)
    Image.open("updated_image.png").save("resized_image.png", format='PNG')
    update_success = True
    if update_success:
        return jsonify({"message": "Tile updated successfully"}), 200
    else:
        return jsonify({"error": "Failed to update tile"}), 500
    
@app.route('/status', methods=["GET"])
def get_status():
    global cache_data
    if cache_data is None:
        return jsonify({"error": "No data available"}), 404

    status = {
        "approved": cache_data.get("approved", False),
        "xloc": cache_data.get("xloc", "Not available"),
        "yloc": cache_data.get("yloc", "Not available")
    }
    return jsonify(status), 200
    