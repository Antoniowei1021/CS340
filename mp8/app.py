import json
from flask import Flask, jsonify, send_file, render_template, request
import requests
import os
import io
import boto3
import base64
import dotenv
import logging
from botocore.exceptions import ClientError
app = Flask(__name__)
INITIAL_STATE = {'colormap':'cividis', 'real':-0.7435, 'imag':0.126129, 'height':0.00018972901232843951, 'dim':256, 'iter':512}
#global variables for the parameters
mand_height = 3.0
mand_dim = 512
mand_iter = 64
mand_color = "twilight"
mand_real = 0.0
mand_imag = 0.0
count = 1
s3 = boto3.client('s3',endpoint_url="http://127.0.0.1:9000", aws_access_key_id="ROOTNAME", aws_secret_access_key="CHANGEME123")
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/all')
def all():
    return render_template('all.html')

@app.route('/moveUp', methods=["POST"])
def mand_moveup():
    global INITIAL_STATE
    INITIAL_STATE["imag"] += INITIAL_STATE["height"] * 0.25
    return "Done!", 200

@app.route('/moveDown', methods=["POST"])
def mand_moveDown():
    global INITIAL_STATE
    INITIAL_STATE["imag"] -= INITIAL_STATE["height"] * 0.25
    return "Done!", 200

@app.route('/moveLeft', methods=["POST"])
def mand_moveLeft():
    global INITIAL_STATE
    INITIAL_STATE["real"] -= INITIAL_STATE["height"] * 0.25
    return "Done!", 200

@app.route('/moveRight', methods=["POST"])
def mand_moveRight():
    global INITIAL_STATE
    INITIAL_STATE["real"] += INITIAL_STATE["height"] * 0.25
    return "Done!", 200

@app.route('/zoomIn', methods=["POST"])
def mand_zoomIn():
    global INITIAL_STATE
    INITIAL_STATE["height"] *= 1 / 1.4
    return "Done!", 200

@app.route('/zoomOut', methods=["POST"])
def mand_zoomOut():
    global INITIAL_STATE
    INITIAL_STATE["height"] *= 1.4
    return "Done!", 200

@app.route('/smallerImage', methods=["POST"])
def mand_small():
    global INITIAL_STATE
    INITIAL_STATE["dim"] *= (1/1.25)
    return "Done!", 200

@app.route('/largerImage', methods=["POST"])
def mand_large():
    global INITIAL_STATE
    INITIAL_STATE["dim"] *= 1.25
    return "Done!", 200

@app.route('/moreIterations', methods=["POST"])
def mand_moreIt():
    global INITIAL_STATE
    INITIAL_STATE["iter"] *= 2
    return "Done!", 200

@app.route('/lessIterations', methods=["POST"])
def mand_lessIt():
    global INITIAL_STATE
    INITIAL_STATE["iter"] *= 0.5
    return "Done!", 200

#different way, do later
@app.route('/changeColorMap', methods=["POST"])
def mand_changeColor():
    global INITIAL_STATE
    desired_color = request.json["colormap"]
    INITIAL_STATE["colormap"] = desired_color
    return "Done!", 200

@app.route("/mandelbrot", methods = ["GET"])
def mandelbrot():
    global mand_color, mand_dim, mand_height, mand_imag, mand_iter, mand_real, count
    #create bucket
    image_filename = 'mandelbrot_image.png'
    try:
        s3.create_bucket(Bucket="mybucket")
        print(s3.list_buckets())
        print("Created `mybucket` bucket!")
    except:
  # bucket already exists
        print("Bucket `mybucket` already exists!")
        # try to download the file using the url, if not, then this is an error, and upload        
        # get the url
    try:
        server_url = os.getenv('MANDELBROT_MICROSERVICE_URL')
        url1= f"{server_url}/mandelbrot/{mand_color}/{mand_real}:{mand_imag}:{mand_height}:{mand_dim}:{mand_iter}"
        url2 = f"{mand_color}/{mand_real}:{mand_imag}:{mand_height}:{mand_dim}:{mand_iter}"
        s3.download_file("mybucket", url2, image_filename)
        print("found")
        return send_file(image_filename, mimetype='image/png', as_attachment=True, attachment_filename=image_filename)
    except Exception as e:
        print("Not found, upload")
        resp = requests.get(url1)
        if resp.status_code == 200:
            s3.upload_fileobj(resp.raw, "mybucket", url2)
            return resp.content
        else:
            return "Failed to fetch image from microservice 1", resp.status_code
    
@app.route('/resetTo', methods=["POST"])
def resetTo():
    global INITIAL_STATE
    NEW_STATE = request.get_json() 
    INITIAL_STATE = NEW_STATE
    return INITIAL_STATE

@app.route('/getState', methods=["GET"])
def getState():
    global INITIAL_STATE
    print(INITIAL_STATE)
    return jsonify(INITIAL_STATE)        
            
@app.route('/clearCache', methods=["GET"])
def clearCache():
    response = s3.list_objects(Bucket='mybucket')
    contents = response["Contents"]
    for image_data in contents:
        s3.delete_object(Bucket='mp8', Key=image_data['Key'])
    return "Done", 200           
                  
@app.route('/storage', methods=["GET"])
def storage():
    # how to enrich this code
    data_set = []
    list_of_objects = s3.list_objects("mybucket")
    for data in list_of_objects['Contents']:
        pair = {
           "key"  : data['Keys'],
           "image" : data
        }
        data_set.append(pair)
    return data_set
               
    
            
            
    