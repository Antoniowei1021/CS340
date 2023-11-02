from flask import Flask, render_template, send_file, request, jsonify
import os
import subprocess

app = Flask(__name__)

# Keep track of the number of successfully extracted GIFs
gif_count = 0

@app.route('/')
def index():
    return render_template("index.html")


@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
    global gif_count

    # Create a temporary directory if it doesn't exist
    if not os.path.exists('temp'):
        os.makedirs('temp')
    
    png_file = request.files.get('png')
    if not png_file:
        return "No file uploaded", 400

    png_path = os.path.join('temp', f"temp_{gif_count}.png")
    png_file.save(png_path)

    gif_path = os.path.join('temp', f"extracted_{gif_count}.gif")
    
    # Execute the png-extractGIF program
    exit_code = subprocess.call(["./png-extractGIF", png_path, gif_path])

    # Check exit code
    if exit_code == 0:
        gif_count += 1

        # Move the GIF to the 'gifs' directory
        if not os.path.exists('gifs'):
            os.makedirs('gifs')
        os.rename(gif_path, os.path.join('gifs', f"{gif_count}.gif"))

        return send_file(os.path.join('gifs', f"{gif_count}.gif"), as_attachment=True, attachment_filename=f"{gif_count}.gif")

    elif exit_code == 1:
        return "The PNG file is invalid.", 422
    else:
        return "The PNG does not contain a hidden GIF.", 415


@app.route('/extract/<int:image_num>', methods=['GET'])
def extract_image(image_num):
    gif_path = os.path.join('gifs', f"{image_num + 1}.gif")
    if os.path.exists(gif_path):
        return send_file(gif_path, as_attachment=True)
    else:
        return "GIF not found", 404


if __name__ == "__main__":
    app.run(debug=True)
