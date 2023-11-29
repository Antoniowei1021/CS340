import random
from re import S
import requests
from flask import Flask, jsonify, render_template, request




response = requests.put('http://fa23-cs340-adm.cs.illinois.edu:5000/addMG', json = {
  "name": "My_maze",
  "url": "http://fa23-cs340-025.cs.illinois.edu:34000/",
  "author": "Eric Wei"
})

app = Flask(__name__)
@app.route('/generate', methods=['GET'])
def gen_maze_segment():
    return jsonify({"geom": ['9aa2aac','5022244','5400444','4446040','5422644','5220264','3260226']})
