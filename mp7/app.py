from flask import Flask, render_template, send_file, request
import os
counter = 0    
app = Flask(__name__)

def create_file():
    global counter
    filename = f"file_{counter}.gif"
    return filename
# Route for "/" for a web-based interface to this micro-service:

@app.route('/')
def index():
  return render_template("index.html")

# Extract a hidden "uiuc" GIF from a PNG image:
@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
  global counter
  r = request.files['png']
  r.save(r.filename)
  os.system('make clean')
  os.system('make')
  f_ = create_file()
  print(f"Executing: ./png-extractGIF {r.filename} {f_}")
  result = os.system(f'./png-extractGIF {r.filename} {f_}')
  if os.path.exists(f_):
    print(f"File {f_} created. Size: {os.path.getsize(f_)} bytes")
  else:
    print(f"File {f_} was not created.")
  result1 = os.waitstatus_to_exitcode(result)
  print(result1)
  if result1 == 0:
    print(f"Sending file: {f_}")
    counter += 1
    return send_file(f_)
  elif result1 == 254:
      return "There's no hidden GIF", 415
  else:
      return "Invalid PNG type", 422

# Get the nth saved "uiuc" GIF:
@app.route('/extract/<int:image_num>', methods=['GET'])
def extract_image(image_num):
    filename = f'file_{image_num}.gif'
    if os.path.exists(filename):
        return send_file(filename)
    return "Not found", 404
  