from flask import Flask, render_template, send_file, request
import os

app = Flask(__name__)
counter = 0  # Counter for naming GIF files
temp_dir = "temp"  # Temporary directory to store files

# Ensure temp directory exists
if not os.path.exists(temp_dir):
    os.makedirs(temp_dir)


@app.route('/')
def index():
    return render_template("index.html")


@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
    global counter

    # Check and save the uploaded PNG file
    uploaded_file = request.files['png']
    if not uploaded_file.filename.endswith('.png'):
        return "Invalid file type uploaded.", 422

    png_path = os.path.join(temp_dir, uploaded_file.filename)
    uploaded_file.save(png_path)

    # Extract GIF from the PNG using the png-extractGIF program
    gif_path = os.path.join(temp_dir, f"file_{counter}.gif")
    exit_code = os.system(f'./png-extractGIF {png_path} {gif_path}')

    # Handle exit codes and responses based on the program's result
    if exit_code == 0:
        counter += 1
        return send_file(gif_path)
    elif exit_code == 1:  # Adjust the exit codes if they are different in your mp2 program
        return "Invalid PNG type", 422
    elif exit_code == 2:  # Adjust the exit codes if they are different in your mp2 program
        return "There's no hidden GIF", 415
    else:
        return "Unexpected error occurred.", 500


@app.route('/extract/<int:image_num>', methods=['GET'])
def extract_image(image_num):
    gif_path = os.path.join(temp_dir, f"file_{image_num}.gif")

    if os.path.exists(gif_path):
        return send_file(gif_path)
    else:
        return "GIF not found", 404


if __name__ == "__main__":
    app.run(debug=True)
