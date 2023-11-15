import random
from re import S
import requests
from flask import Flask, jsonify, render_template, request




response = requests.put('http://127.0.0.1:5001/addMG', json = {
  "name": "My_maze",
  "url": "http://127.0.0.1:5002/",
  "author": "Eric Wei"
})

print(response.status_code)

app = Flask(__name__)
@app.route('/generate', methods=['GET'])
def gen_maze_segment():
    return jsonify({"geom": ['9aa2aac','5022244','5400444','4446040','5422644','5220264','3260226']})
