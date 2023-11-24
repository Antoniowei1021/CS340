import requests
from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

servers = []

@app.route('/registered', methods=["PUT"])
    # puts a json with
    # authtoken : auth token from the server
    # approved : the string true or false
    # voteToken : auth token for voting if approved is true
    # xloc : int //tile location
    # yloc : in // tile location

    # 455 "Authorization token invalid" if auth token does not match

    # otherwise return 200 "success"
def PUT_registered()
