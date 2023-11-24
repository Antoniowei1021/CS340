import requests
from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

servers = []

@app.route('/registerClient/<clientID>', methods=["PUT"])
def PUT_registerClient(<clientID>)
    # this puts a json with the following
    # token : auth token for the server
    # url : "http://server:port/"
    # author : "Your Name"

    # if clientID is in use return 415 "ClientID already exists"

    # post json with microservice ip and port

    # otherwise return 200 "success" and json with
    # xdim : int // in tiles
    # ydim : int // in tiles
    # tilesize : int

@app.route('/registerImage/<clientID>', methods=["POST"])
def POST_registerImage(<clientID>)
    # post png image of correct size

    # 500 "Image size invalid" if image does not match required size
    # 416 "ClientID not registered" if client was not registered
    
    # otherwise return 200 "success"
    