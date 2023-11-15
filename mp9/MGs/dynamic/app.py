import random
from re import S
import requests
from flask import Flask, jsonify, render_template, request
# Re-creating the DisjointSet and SquareMaze classes
class DisjointSet:
    def __init__(self, size):
        self.parent = list(range(size))
    
    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]

    def union(self, x, y):
        root_x = self.find(x)
        root_y = self.find(y)
        if root_x != root_y:
            self.parent[root_y] = root_x

class SquareMaze:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.maze = [[{'right': True, 'down': True} for _ in range(width)] for _ in range(height)]
        self.disjSet = DisjointSet(width * height)

    def makeMaze(self):
        count = 1
        while count < self.width * self.height:
            for y in range(self.height):
                for x in range(self.width):
                    random_wall = random.randint(0, 1)
                    if random_wall == 0 and x < self.width - 1:
                        if self.disjSet.find(y * self.width + x) != self.disjSet.find(y * self.width + x + 1):
                            self.setWall(x, y, 0, False)
                            self.disjSet.union(y * self.width + x, (y * self.width + x) + 1)
                            count += 1
                    elif random_wall == 1 and y < self.height - 1:
                        if self.disjSet.find(y * self.width + x) != self.disjSet.find((y + 1) * self.width + x):
                            self.setWall(x, y, 1, False)
                            self.disjSet.union(y * self.width + x, (y + 1) * self.width + x)
                            count += 1

        # Adding exits in the middle of each side after the maze is generated
        middle_x = self.width // 2
        middle_y = self.height // 2
        self.setWall(middle_y, 0, 1, False) 
        self.setWall(middle_y, self.width - 1, 1, False)  
        self.setWall(0, middle_x, 0, False) 
        self.setWall(self.height - 1, middle_x, 0, False)  

    def setWall(self, x, y, dir, exists):
        if dir == 0:
            self.maze[y][x]['right'] = exists
        elif dir == 1:
            self.maze[y][x]['down'] = exists

def maze_to_string(maze):
    geom = []
    for y in range(len(maze)):
        row = ""
        for x in range(len(maze[0])):
            cell_value = 0
            if y == 0 or (y > 0 and maze[y - 1][x]['down']):
                cell_value |= 8
            if x == len(maze[0]) - 1 or maze[y][x]['right']:
                cell_value |= 4
            if y == len(maze) - 1 or maze[y][x]['down']:
                cell_value |= 2
            if x == 0 or (x > 0 and maze[y][x - 1]['right']):
                cell_value |= 1
            # Adjust the wall encoding for exits
            middle_x = len(maze[0]) // 2
            middle_y = len(maze) // 2
            if y == 0 and x == middle_x:
                cell_value &= ~8 
            if y == len(maze) - 1 and x == middle_x:
                cell_value &= ~2  
            if x == 0 and y == middle_y:
                cell_value &= ~1  
            if x == len(maze[0]) - 1 and y == middle_y:
                cell_value &= ~4 

            row += hex(cell_value)[-1]
        geom.append(row)
    return {"geom": geom}


# Generate the maze and convert it to string
servers = []
names = []
weights = []



response = requests.put('http://127.0.0.1:5001/addMG', json = {
  "name": "My_maze",
  "url": "http://127.0.0.1:5002/",
  "author": "Eric Wei"
})
# lists of MG names and weights for random.choices
app = Flask(__name__)



@app.route('/generate', methods=['GET'])
def gen_maze_segment():

    maze = SquareMaze(7, 7)
    maze.makeMaze()
    maze_string = maze_to_string(maze.maze)
    return jsonify(maze_string), 200
    