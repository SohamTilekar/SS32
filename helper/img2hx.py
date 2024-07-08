import tkinter as tk
from tkinter import filedialog
from PIL import Image

def image_to_pixel_values(image_path, output_file):
    # Open the image
    with Image.open(image_path) as img:
        # Resize the image to 256x256
        img = img.resize((256, 256))
        # Ensure the image is in RGB mode
        img = img.convert('RGB')
        # Get the width and height of the image
        width, height = img.size
        # Open the output file for writing in binary mode
        with open(output_file, 'w') as file:
            # Iterate over each pixel in the image
            for y in range(height):
                for x in range(width):
                    # Get the RGB values of the pixel
                    r, g, b = img.getpixel((x, y))
                    # Convert the RGB values to hex format
                    hex_value = "00" + format(r, '02X') + format(g, '02X') + format(b, '02X') + "\n"
                    # Write the hex value to the file
                    file.write(hex_value)

def main():
    # Create a tkinter root window
    root = tk.Tk()
    root.withdraw()  # Hide the root window

    # Ask the user to select the input image file
    image_path = filedialog.askopenfilename(
        title="Select an Image File",
        filetypes=[("Image files", "*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.webp")]
    )
    if not image_path:
        print("No image file selected.")
        return

    # Ask the user to select the output text file
    output_file = filedialog.asksaveasfilename(
        title="Save Output File",
        defaultextension=".txt",
        filetypes=[("Text files", "*.txt"), ("Hex Files", "*.hex")]
    )
    if not output_file:
        print("No output file selected.")
        return

    # Process the image and write the pixel values to the output file
    image_to_pixel_values(image_path, output_file)
    print(f"Pixel values have been written to {output_file}")

if __name__ == "__main__":
    main()
